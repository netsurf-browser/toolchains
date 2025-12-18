#include <string.h>

#include "oslib/os.h"
#include "oslib/netfs.h"

os_error *read_file_server_version (void)

{  os_error *error = NULL;
   int rx_count;
   netfs_read_version version;

   if ((error = xnetfs_do_fs_op (netfs_FS_OP_READ_VERSION,
         (netfs_op *) &version, 0, sizeof version, NULL, &rx_count))
         != NULL)
      goto finish;
   version AS reply.version [rx_count] = '\0';

   if ((error = xos_write0 (version AS reply.version)) != NULL)
      goto finish;
   if ((error = xos_new_line ()) != NULL)
      goto finish;

finish:
   return error;
}

os_error *print_station_number_of_user (char *user)

{  os_error *error = NULL;
   os_station_number station_number;
   netfs_read_user_info user_info;
   char net_station [os_NET_STATION_LIMIT + 1];

   strcpy (user_info AS request.user_name, user);
   user_info AS request.user_name
         [strlen (user_info AS request.user_name)] = '\r';

   if ((error = xnetfs_do_fs_op (netfs_FS_OP_READ_USER_INFO,
         (netfs_op *) &user_info, 0, sizeof user_info, NULL, NULL))
         != NULL)
      goto finish;

   station_number.net     = user_info AS reply.net;
   station_number.station = user_info AS reply.station;

   if ((error = xos_convert_net_station (&station_number, net_station,
         os_NET_STATION_LIMIT, NULL)) != NULL)
      goto finish;

   if ((error = xos_write0 (net_station)) != NULL)
      goto finish;
   if ((error = xos_new_line ()) != NULL)
      goto finish;

finish:
   return error;
}
