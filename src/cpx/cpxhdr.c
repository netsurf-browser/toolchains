/*
 * cpxhdr : small utilitie to create an CPX 512-byte header
 * by Dominique B‚r‚ziat
 * (c) 2004
 * $Id: cpxhdr.c 27 2005-09-10 21:21:38Z bereziat $
 */

#define __GEMLIB_OLDNAMES
#include <gem.h>
#include <stdio.h>
#include <string.h>

struct foobar {
	int dummy;
};

#include "xcontrol.h"

/* The Atari CPX header definition */

/*
typedef struct {
  short magic;
  struct {
    unsigned reserved : 13;
    unsigned resident : 1;
    unsigned boot_init : 1;
    unsigned set_only : 1;
  } flags;
  char cpx_id[4];
  short cpx_version;
  char icon_name[14];
  long icon_data[24];
  short icon_info;
  char cpx_name[18];
  short obj_state;
  char reserved[370];
} CPX_HEADER; 
*/


/*
 * Init the CPX header from reading a resource file
 * Resource file has :
 *  - an icon as object #1 : it is used as the CPX icon in Xcontrol
 *  - an tedinfo as object #2 : 
 *    + the 4 first bytes are used as the CPX id
 *    + Next bytes are used a the CPX version until a white space is encoured.
 *    + Last bytes after this white space is used as the CPX title
 *  - root object flags defined some CPX flags :
 *    + SELECTABLE means the cpx is 'resident',
 *    + DEFAULT means the cpx is 'boot_init'
 *    + EXIT means the cpx is 'set_only'
 */

int set_header( OBJECT *tree, CPX_HEADER *header) {
  TEDINFO *tedinfo;
  ICONBLK *iconblk;
  char *p;
  long *buff;
  int i;
  
  /* TODO : check validity of objects by their ob_type */

  iconblk = tree[1].ob_spec.iconblk;
  tedinfo = tree[2].ob_spec.tedinfo;
  
  header -> magic = 100;
  header -> flags.resident  = (tree[0].ob_flags&SELECTABLE)?1:0;
  header -> flags.boot_init = (tree[0].ob_flags&DEFAULT)?1:0;
  header -> flags.set_only  = (tree[0].ob_flags&EXIT)?1:0;
  memcpy( header -> cpx_id, tedinfo->te_ptext, 4);
  
  sscanf  ( tedinfo->te_ptext+4, "%x", &(header->cpx_version));
  strncpy ( header -> icon_name, iconblk->ib_ptext, 14);
  p = strchr( tedinfo->te_ptext+5, ' ')+1;
  strncpy( header->cpx_name, p, 17);
  
  header -> icon_info = iconblk->ib_char;
  header -> obj_state = tedinfo->te_color;
  
  buff = (long*)iconblk->ib_pdata;
  for( i=0; i<24; i++)
    header->icon_data[i] = buff[i];

  return 1;
}

int main( int argc, char *argv[]) {
  if( argc >= 3 ) {    
    appl_init();   
    if( rsrc_load( argv[1]) ) {
      FILE *fp;
      CPX_HEADER header;
      OBJECT *tree;

      rsrc_gaddr( 0, 0, &tree);
      set_header( tree, &header);

      fp = fopen( argv[2], "wb");
      if( fp) {
		fwrite( &header, sizeof(header), 1, fp);
		fclose( fp);
      } else
		fprintf( stderr, "Fatal : can not write %s\n", argv[2]);
      
    } else
      fprintf( stderr, "Fatal : '%s' is not a resource file\n", argv[1]);
    
  } else
    fprintf( stderr, "Usage is : %s cpx-resource-file cpx-header\n", argv[0]);
  appl_exit();
  return 0;
}
