#include "internal/cryptlib.h"
#include <openssl/rand.h>
#include <openssl/sha.h>
#include "crypto/rand_pool.h"
#include "crypto/rand.h"
#include "prov/seeding.h"

#if defined(OPENSSL_SYS_AMIGAOS3) || defined(OPENSSL_SYS_AMIGAOS4)

# ifndef OPENSSL_RAND_SEED_OS
#  error "Unsupported seeding method configured; must be os"
# endif

#define __USE_INLINE__ 1

#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <proto/exec.h>
#include <proto/timer.h>
#include <devices/timer.h>

/* Maximum number of attempts to get a delay of 1 microsecond that is not equal to 0 */
#define MAX_ATTEMPTS 1000

#ifdef __amigaos4__

#ifdef CreateMsgPort
#undef CreateMsgPort
#endif
#define CreateMsgPort() AllocSysObject(ASOT_PORT,NULL)
#ifdef DeleteMsgPort
#undef DeleteMsgPort
#endif
#define DeleteMsgPort(msgPort) FreeSysObject(ASOT_PORT,msgPort)
#ifdef CreateIORequest
#undef CreateIORequest
#endif
#define CreateIORequest(ioReplyPort,size) AllocSysObjectTags(ASOT_IOREQUEST,ASOIOR_ReplyPort,ioReplyPort,ASOIOR_Size,size,TAG_DONE)
#ifdef DeleteIORequest
#undef DeleteIORequest
#endif
#define DeleteIORequest(ioReq) FreeSysObject(ASOT_IOREQUEST,ioReq)

#else

#define GetInterface(a, b, c, d) 1
#define DropInterface(x)

/* OS3 has a different but compatible TimeVal definition */
struct TimeVal
{
    uint32 Seconds;
    uint32 Microseconds;
};

#endif /* !__amigaos4__ */

size_t ossl_pool_acquire_entropy(RAND_POOL *pool)
{
    unsigned char temp_buffer[SHA_DIGEST_LENGTH], data_buffer[SHA_DIGEST_LENGTH];
    struct MsgPort *port = NULL;
    size_t bytes_needed;
    size_t entropy_available = 0;
    struct TimeRequest *time_request = NULL;
#ifdef __amigaos4__
    struct IOStdReq *entropy_request = NULL;

    bytes_needed = ossl_rand_pool_bytes_needed(pool, 1);

    if ((port = CreateMsgPort())
        && (entropy_request = (struct IOStdReq *)CreateIORequest(port, sizeof(*entropy_request))))
    {
        if (OpenDevice(TIMERNAME, UNIT_ENTROPY, (struct IORequest *)entropy_request, 0) == 0)
        {
            while(bytes_needed > 0)
            {
                entropy_request->io_Command = TR_READENTROPY;
                entropy_request->io_Data = &temp_buffer[0];
                entropy_request->io_Length = sizeof(temp_buffer);

                if (DoIO((struct IORequest *)entropy_request) == 0)
                {
                    unsigned char *buffer;
                    size_t bytes = (bytes_needed < sizeof(data_buffer) ? bytes_needed : sizeof(data_buffer));

                    buffer = ossl_rand_pool_add_begin(pool, bytes_needed);
                    SHA1(&temp_buffer[0], sizeof(temp_buffer), &data_buffer[0]);
                    memcpy(buffer, data_buffer, bytes);
                    ossl_rand_pool_add_end(pool, bytes, 8 * bytes);
                    bytes_needed -= bytes;
                }
                else
                    break;
            }

            CloseDevice((struct IORequest *)entropy_request);
        }
    }

    DeleteIORequest((struct IORequest *)entropy_request);

    entropy_available = ossl_rand_pool_entropy_available(pool);
    if (entropy_available > 0)
        return entropy_available;
#endif /* __amigaos4__ */

    /* The following block will be used on "classic" machines. It does not generate
     * a high degree of randomness, but it does the job since RAND_poll is
     * called only once by OpenSSL to generate a 32 byte seed.
     */

    bytes_needed = ossl_rand_pool_bytes_needed(pool, 1);

    if (bytes_needed > 0
        && (port || (port = CreateMsgPort()))
        && (time_request = (struct TimeRequest *)CreateIORequest(port, sizeof(*time_request))))
    {
        if (OpenDevice(TIMERNAME, UNIT_VBLANK, (struct IORequest *)time_request, 0) == 0)
        {
            #if defined(__amigaos4__)
            struct TimerIFace *ITimer = NULL;
            #endif
            struct Device *TimerBase;

            if ((TimerBase = time_request->Request.io_Device)
            #if defined(__amigaos4__)
                && (ITimer = (struct TimerIFace *)GetInterface((struct Library *)TimerBase, "main", 1, NULL))
            #endif
            )
            {
                struct EClockVal curr_eclock;
                ULONG prev_ev_lo = 0;
                struct TimeVal tv;
                int i, attempt;
                BOOL aborted;

                ReadEClock(&curr_eclock);
                aborted = FALSE;

                while(!aborted && bytes_needed > 0)
                {
                    for(i = 0;
                        !aborted && i < (int)sizeof(temp_buffer) - (int)sizeof(ULONG);
                        i++)
                    {
                        attempt = 0;

                        /* Ask for a one microsecond delay and measure the time
                         * the delay actually took.
                         */
                        do
                        {
                            time_request->Request.io_Command = TR_ADDREQUEST;
                            time_request->Time.Seconds = 0;
                            time_request->Time.Microseconds = 1;

                            if (DoIO((struct IORequest *)time_request) == 0)
                            {
                                prev_ev_lo = curr_eclock.ev_lo;
                                ReadEClock(&curr_eclock);

                                attempt++;
                            }
                            else
                                aborted = TRUE;
                        } while(!aborted && prev_ev_lo == 0 && attempt < MAX_ATTEMPTS);

                        if (attempt >= MAX_ATTEMPTS)
                            aborted = TRUE;

                        /* Since we are going for randomness, ev_hi is irrelevant */
                        temp_buffer[i] = (unsigned char)(curr_eclock.ev_lo - prev_ev_lo);
                    }

                    GetSysTime(&tv);

                    if (sizeof(temp_buffer) > sizeof(ULONG))
                        *(ULONG *)&temp_buffer[sizeof(temp_buffer) - sizeof(ULONG)]
                            = tv.Microseconds;

                    /* Shuffle the bits around and specify that about
                     * one fourth of it adds to the entropy.
                     */
                    if (!aborted)
                    {
                        unsigned char *buffer;
                        size_t bytes = (bytes_needed < sizeof(data_buffer) ? bytes_needed : sizeof(data_buffer));

                        SHA1(&temp_buffer[0], sizeof(temp_buffer), &data_buffer[0]);

                        ossl_rand_pool_add_end(pool, bytes, (sizeof(data_buffer) / 4) * 8);
                        bytes_needed -= bytes;
                    }
                }
            }

            #if defined(__amigaos4__)
            DropInterface((struct Interface *)ITimer);
            #endif
            CloseDevice((struct IORequest *)time_request);
        }
    }

    DeleteIORequest((struct IORequest *)time_request);

    DeleteMsgPort(port);

    return ossl_rand_pool_entropy_available(pool);
}

int ossl_pool_add_nonce_data(RAND_POOL *pool)
{
    struct {
        pid_t pid;
        CRYPTO_THREAD_ID tid;
        uint64_t time;
    } data;

    /* Erase the entire structure including any padding */
    memset(&data, 0, sizeof(data));

    /*
     * Add process id, thread id, and a high resolution timestamp to
     * ensure that the nonce is unique with high probability for
     * different process instances.
     */
    data.pid = getpid();
    data.tid = CRYPTO_THREAD_get_current_id();
    /*XXX: can this be improved? */
    data.time = time(NULL);

    return ossl_rand_pool_add(pool, (unsigned char *)&data, sizeof(data), 0);
}

int ossl_rand_pool_add_additional_data(RAND_POOL *pool)
{
    struct {
        int fork_id;
        CRYPTO_THREAD_ID tid;
        uint64_t time;
    } data;

    /* Erase the entire structure including any padding */
    memset(&data, 0, sizeof(data));

    /*
     * Add some noise from the thread id and a high resolution timer.
     * The fork_id adds some extra fork-safety.
     * The thread id adds a little randomness if the drbg is accessed
     * concurrently (which is the case for the <master> drbg).
     */
    data.fork_id = openssl_get_fork_id();
    data.tid = CRYPTO_THREAD_get_current_id();
    /*XXX: can this be improved? */
    data.time = (uint64_t) time(NULL);

    return ossl_rand_pool_add(pool, (unsigned char *)&data, sizeof(data), 0);
}

int ossl_rand_pool_init(void)
{
    return 1;
}

void ossl_rand_pool_cleanup(void)
{
}

void ossl_rand_pool_keep_random_devices_open(int keep)
{
}

#endif /* OPENSSL_SYS_AMIGA */
