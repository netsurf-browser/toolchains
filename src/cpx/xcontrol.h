/* XCONTROL.H from "ST-Computer" 3/1991
 * modif by Bereziat - 12/2004*/

#ifndef _XCONTROL_
#define _XCONTROL_

#ifdef __PUREC__
#define __CDECL cdecl
#endif

typedef struct
{
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

typedef struct {
	short flags;
	short x,y,w,h;
} CPX_MOUSE_EVENT;

typedef struct {
	short mx,my;
	short mbutton;
	short kbstate;
} CPX_MOUSE_RET;

typedef struct {
	short __CDECL (*cpx_call)(GRECT *work);
	void __CDECL (*cpx_draw)(GRECT *clip);
	void __CDECL (*cpx_wmove)(GRECT *work);
	void __CDECL (*cpx_timer)(short *event);
	void __CDECL (*cpx_key)(short kbstate,short key,short *event);
	void __CDECL (*cpx_button)(CPX_MOUSE_RET *mrets,short nclicks,short *event);
	void __CDECL (*cpx_m1)(CPX_MOUSE_RET *mrets,short *event);
	void __CDECL (*cpx_m2)(CPX_MOUSE_RET *mrets,short *event);
	short __CDECL (*cpx_evhook)(short event,short *msgbuff,CPX_MOUSE_RET *mrets,short *key,short *nclicks);
	void __CDECL (*cpx_close)(short app_term);
} CPX_INFO;

typedef struct {
	void *text_start;
	long text_len;
	void *data_start;
	long data_len;
	void *bss_start;
	long bss_len;
} CPX_SEGMENTS;

typedef struct cpxblock {
	char filename[14];
	short ok;
	short valid;
	CPX_SEGMENTS *segments;
	struct cpxblock *next;
	CPX_HEADER header;
} CPX_BLOCK;

typedef struct {
	short vdi_handle;
	short booting;
	short reserved;
	short rsc_init;
	CPX_BLOCK * __CDECL (*get_rootblock)(void);
	short __CDECL (*write_header)(CPX_BLOCK *header);
	void __CDECL (*do_resource)(short num_obs,short num_frstr,short num_frimg,short num_tree,
						OBJECT *rs_object,TEDINFO *rs_tedinfo,char *rs_strings[],ICONBLK *rs_iconblk,
						BITBLK *rs_bitblk,char *rs_frstr,short *rs_frimg,OBJECT **rs_trindex,struct foobar *rs_imdope);
	void __CDECL (*objc_adjust)(OBJECT *tree,short ob_index);
	short __CDECL (*do_pulldown)(char *entries[],short num_items,short checked_item,short font,GRECT *button_xywh,GRECT *window_xywh);
	void __CDECL (*size_slider)(OBJECT *tree,short box_index,short slider_index,short total,short seen,short v_h_flag,short min_size);
	void __CDECL (*pos_hslider)(OBJECT *tree,short box_index,short slider_index,short slider_pos,short start,short total,void (*function)());
	void __CDECL (*pos_vslider)(OBJECT *tree,short box_index,short slider_index,short slider_pos,short start,short total,void (*function)());
	void __CDECL (*inc_slider)(OBJECT *tree,short box_index,short slider_index,short button_index,short increment,short start,short total,short *slider_pos, short v_h_flag,void (*function)() );
	void __CDECL (*move_hslider)(OBJECT *tree,short box_index,short slider_index,short start,short total,short *slider_pos,void (*function)() );
	void __CDECL (*move_vslider)(OBJECT *tree,short box_index,short slider_index,short start,short total,short *slider_pos,void (*function)() );
	short __CDECL (*do_form)(OBJECT *tree,short start_objc,short *msg_buffer);
	GRECT *__CDECL (*rci_first)(GRECT *object_xywh);
	GRECT *__CDECL (*rci_next)(void);
	void __CDECL (*multi)(short ev_flags,CPX_MOUSE_EVENT *mm1,CPX_MOUSE_EVENT *mm2,long timer);
	short __CDECL (*alert)(short number);
	short __CDECL (*write_config)(void *parameter,long length);
	char *__CDECL(*get_resarea)(void);
	short __CDECL (*find_cookie)(long cookie,long *version);
	short dummy;
	void __CDECL (*copy_bltparam)(short dir,void *buffer);
} CPX_PARAMS;

#define LARGE 3
#define SMALL 5
#define	CT_KEY 53

#endif
