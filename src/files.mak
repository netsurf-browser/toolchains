# WinDom: a high level GEM library
# Copyright (c) 1997-2006 windom authors (see AUTHORS file)
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# $Source: /cvsroot/windom/windom/src/files.mak,v $
# list of files used in various makefile
# 
# CVS Info :
#  $Author: opichals $
#  $Date: 2006/02/22 18:33:33 $
#  $Revision: 1.23 $

# 
# hierarchy used:
#
# ..             -- root directory
# ../src         -- here we are
# ../include     -- headers files
# ../lib/gcc     -- lib compiled for gcc 2.95.3 (SpareMiNT)
# ../lib/gcc281  -- lib compiled for gcc 2.81c
# ../lib/sozobon -- lib compiled for sozobonx
# ../lib/purec   -- lib compiled for pure c
#

# library filenames

LIB_GCC           = ../lib/gcc/libwindom.a
LIB_GCC281        = ../lib/gcc281/windom.olb
LIB_SOZOBON       = ../lib/sozobon/windom.a
LIB_PUREC         = ../lib/purec/windom.lib

# list of objects

KERNEL= \
	appl_get.o appl_set.o appl_init.o appl_exit.o appl_name.o appl_write.o\
	appl_control.o \
	std_evnt.o std_mwk.o std_fntchg.o std_dstry.o move_work.o\
	evnt_list.o evnt_redraw.o evnt_windom.o \
	snd_rdw.o snd_arrw.o snd_msg.o\
	give_iconifyxywh.o is_modal.o do_update.o\
	wind_lst.o wind_lst2.o wind_slider.o wind_clear.o wind_create.o\
	wind_open.o wind_handle.o wind_close.o wind_delete.o wind_calc.o\
	wind_set.o wind_get.o wind_new.o wind_attach.o wind_find.o wind_top.o\
	form_close.o frm_buttn_ev.o frm_keybd_ev.o get_next_obj.o\
	obj_fd_flag.o obj_fd_xtype.o obj_nb.o obj_root.o \
	objc_attach.o objc_change.o objc_drawparent.o objc_dup.o objc_edit.o\
	objc_free.o objc_str3d.o objc_strfmt.o objc_string.o objc_char.o objc_ishidden.o\
	objc_wchange.o objc_wdraw.o objc_wdrawparent.o obspec_mem.o\
	draw_submenu.o gemcode2ascii.o getindexmenu.o is_menu.o\
	menu_attach.o menu_bar.o menu_bind.o menu_disable.o\
	menu_enable.o menu_execcmd.o menu_icheck.o menu_scroll.o\
	menu_text.o menu_tnormal.o mnscroll.o\
	rsrc_ghdr.o rsrc_gaddr.o rsrc_xfree.o rsrc_load.o rsrc_xload.o\
	objc_tree.o slid_create.o slid_delete.o slid_attach.o slid_getvalue.o\
	slid_setvalue.o slid_setfunc.o slid_setsize.o slid_setupdat.o \
	mouse_objc.o mouse_sprite.o mouse_work.o\
	rc_set.o clip_off.o clip_on.o w_getpal.o conv_path.o\
	scrap_txtwrite.o scrap_txtread.o w_putbkgr.o galloc.o\
	w_setpal.o w_getbkgr.o keybd2ascii.o\
	rsrc_userdraw.o rsrc_userfree.o xrsrc.o\
	form_alert.o form_attach.o form_begin.o form_create.o\
	form_end.o form_resize.o form_restore.o form_save.o\
	form_thbget.o form_thbset.o form_thumb.o\
	form_wbegin.o form_wdo.o form_wend.o\
	frm_click.o frm_drw.o frm_dstry.o frm_fld.o\
	frm_keyhd.o frm_mvd.o frm_top.o\
	frame_attach.o frame_calc.o frame_create.o frame_find.o\
	frame_get.o frame_init.o frame_remove.o frame_search.o\
	frame_set.o frame_set_widget.o remove_frame.o\
	set_gadget_pos.o\
	fsel.o \
	data.o \
	conf_windom.o conf_write.o get_appname.o conf_getline.o\
	conf_read.o add_slash.o conf_inquire.o\
	shel_help.o grectcenter.o shel_write.o debug_windom.o \
	font_sel.o \
	cicon.o \
	vqt_xname.o load_fontid.o close_fontid.o vst_loadfonts.o vst_unloadfonts.o\
	vst_font.o vqt_name.o font_name2id.o font_id2name.o\
	bubble_get.o snd_mubutton.o bubble_modal.o bubble_do.o bubble_call.o\
	bubble_attach.o bubble_free.o bubble_find.o bubble_evnt.o\
	av_init.o av_waitfor.o av_exit.o av_status.o av_server.o av_strfmt.o\
	popup.o \
	cookieptr.o get_cookie.o \
	vq_extfs.o vq_mint.o\
	vq_magx.o vqt_extname.o \
	globals.o \
	glue.o \
	\
	udlib_init.o udlib_exit.o udlib_extended.o udlib_extfree.o udlib_objfind.o\
	udlib_string.o udlib_char.o udlib_edit.o udlib_get_edpos.o udlib_add_obj.o udlib_get_type.o\
	udlib_rm_obj.o udlib_unextended.o udlib_garbage.o udlib_list.o\
	list.o ob_istype.o \
	\
	comp_create.o comp_szd.o \
	comp_window.o comp_aslider.o \
	comp_object.o \
	comp_split.o comp_slider.o comp_menu.o \
	comp_widget.o \
	\
	xobjc_char.o xobjc_draw.o xobjc_extend.o xobjc_interface.o xobjc_string.o xobjc_xedit.o


WINDOMOBJ = $(KERNEL) $(EXTRAOBJ)
