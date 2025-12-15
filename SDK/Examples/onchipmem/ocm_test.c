/*  Copyright (c) 2013, ACube Systems Srl
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or
 *  without modification, are permitted provided that the following
 *  conditions are met:
 *
 *    * Redistributions of source code must retain the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer.
 *    * Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials
 *      provided with the distribution.
 *    * Neither the name of ACube Systems Srl nor the names of
 *      its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written
 *      permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 *  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 *  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 *  OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 *  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <resources/onchipmem.h>
#include <proto/exec.h>
#include <proto/onchipmem.h>
#include <proto/dos.h>
#include <proto/timer.h>

struct Device *TimerBase;
struct TimerIFace *ITimer;

struct TimeRequest *tr;
struct MsgPort *tmsg;
BYTE tderror;
struct TimeVal tv1, tv2;

BOOL OpenTimer(void)
{
    if (tmsg=IExec->AllocSysObjectTags(ASOT_PORT, TAG_END))
    {
        if (tr=(struct TimeRequest *)IExec->AllocSysObjectTags(ASOT_IOREQUEST,ASOIOR_Size,sizeof(struct TimeRequest),ASOIOR_ReplyPort,tmsg))
        {
            tderror=IExec->OpenDevice("timer.device", UNIT_ECLOCK, (struct IORequest *)tr, 0);
            if(!tderror)
            {
                TimerBase=(struct Device *)&tr->Request.io_Device->dd_Library;
                ITimer = (struct TimerIFace *)IExec->GetInterface((struct Library*)TimerBase, "main", 1, NULL);

                return(TRUE);
            }
        }
    }
    
    return(FALSE);
}

void CloseTimer(void)
{
    if (ITimer != NULL) IExec->DropInterface((struct Interface *)ITimer);
    if (TimerBase) IExec->CloseDevice((struct IORequest *)tr);
    if (tr)        IExec->FreeSysObject(ASOT_IOREQUEST,tr);
    if (tmsg)      IExec->FreeSysObject(ASOT_PORT,tmsg);
}

int main(void)
{
    uint32 ii;
    UBYTE *src = NULL;
    struct Library *ocmb = IExec->OpenResource("onchipmem.resource");
    struct OCMIFace *IOCM = NULL;
    double timediff, speed;

    if (OpenTimer())
    {
        if (ocmb) IOCM = (struct OCMIFace *)IExec->GetInterface((struct Library *)ocmb, "main", 1, NULL);

        if (IOCM != NULL)
        {
            printf("OCM opened\n");

            uint8 *ocm = (uint8 *)IOCM->ObtainOnChipMem();

            printf("OCM address = %p\n",ocm);

            if (ocm != NULL)
            {
                src = (void *)IExec->AllocVecTags(65536, AVT_Type, MEMF_PRIVATE, AVT_PhysicalAlignment, 8, TAG_END);

                if (src)
                {
                    ocm[0] = 0x11;
                    ocm[65535] = 0x22;

                    src[0] = 0x5a;
                    src[65535] = 0xa5;

                    IExec->Forbid();
                    ITimer->GetSysTime(&tv1);

                    IOCM->CopyOnChipMem((uint32 *)ocm, (uint32 *)src, 65536);

                    ITimer->GetSysTime(&tv2);
                    IExec->Permit();

                    ITimer->SubTime(&tv2,&tv1);

                    timediff = ((double)tv2.Seconds * 1000000.0) + (double)tv2.Microseconds;
                    timediff = timediff / 1000000.0;
                    speed = 65536 / timediff / (1024 * 1024);

                    printf("Time  : %f\n",timediff);
                    printf("OCM -> RAM Speed : %f\n",speed);

                    printf("%x %x\n",ocm[0],ocm[65535]);
                    printf("%x %x\n",src[0],src[65535]);

                    //--------------------------------------------------------

                    ocm[0] = 0x11;
                    ocm[65535] = 0x22;

                    src[0] = 0x5a;
                    src[65535] = 0xa5;

                    IExec->Forbid();
                    ITimer->GetSysTime(&tv1);

                    IOCM->CopyOnChipMem((uint32 *)src, (uint32 *)ocm, 65536);

                    ITimer->GetSysTime(&tv2);
                    IExec->Permit();

                    ITimer->SubTime(&tv2,&tv1);

                    timediff = ((double)tv2.Seconds * 1000000.0) + (double)tv2.Microseconds;
                    timediff = timediff / 1000000.0;
                    speed = 65536 / timediff / (1024 * 1024);

                    printf("Time  : %f\n",timediff);
                    printf("RAM -> OCM Speed : %f\n",speed);

                    printf("%x %x\n",ocm[0],ocm[65535]);
                    printf("%x %x\n",src[0],src[65535]);

                    IExec->FreeVec(src);
                }
            }

            IOCM->ReleaseOnChipMem();

            IExec->DropInterface((struct Interface *)IOCM);
        }
        CloseTimer();
    }

    return 0;
}
