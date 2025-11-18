--- include/clib/guigfx_protos.h	1999-05-23 19:36:26.000000000 +0100
+++ include/clib/guigfx_protos.h	2025-11-18 15:17:35.356000000 +0000
@@ -13,22 +13,22 @@
 #endif
 
 
-APTR MakePicture(APTR array, UWORD width, UWORD height, ...);
+APTR MakePicture(APTR array, UWORD width, UWORD height, Tag tag1, ...);
 APTR MakePictureA(APTR array, UWORD width, UWORD height, struct TagItem *tags);
-APTR LoadPicture(STRPTR filename, ...);
+APTR LoadPicture(STRPTR filename, Tag tag1, ...);
 APTR LoadPictureA(STRPTR filename, struct TagItem *tags);
-APTR ReadPicture(struct RastPort *rp, struct ColorMap *cm, UWORD x, UWORD y, UWORD width, UWORD height, ...);
+APTR ReadPicture(struct RastPort *rp, struct ColorMap *cm, UWORD x, UWORD y, UWORD width, UWORD height, Tag tag1, ...);
 APTR ReadPictureA(struct RastPort *rp, struct ColorMap *cm, UWORD x, UWORD y, UWORD width, UWORD height, struct TagItem *tags);
-APTR ClonePicture(APTR pic, ...);
+APTR ClonePicture(APTR pic, Tag tag1, ...);
 APTR ClonePictureA(APTR pic, struct TagItem *tags);
 
 void DeletePicture(APTR pic);
 
-APTR AddPicture(APTR psm, APTR pic, ...);
+APTR AddPicture(APTR psm, APTR pic, Tag tag1, ...);
 APTR AddPictureA(APTR psm, APTR pic, struct TagItem *tags);
-APTR AddPalette(APTR psm, APTR palette, ...);
+APTR AddPalette(APTR psm, APTR palette, Tag tag1, ...);
 APTR AddPaletteA(APTR psm, APTR palette, struct TagItem *tags);
-APTR AddPixelArray(APTR psm, APTR array, UWORD width, UWORD height, ...);
+APTR AddPixelArray(APTR psm, APTR array, UWORD width, UWORD height, Tag tag1, ...);
 APTR AddPixelArrayA(APTR psm, APTR array, UWORD width, UWORD height, struct TagItem *tags);
 
 void RemColorHandle(APTR colorhandle);
@@ -37,45 +37,45 @@ APTR CreatePenShareMap(Tag tag1, ...);
 APTR CreatePenShareMapA(struct TagItem *tags);
 void DeletePenShareMap(APTR psm);
 
-APTR ObtainDrawHandle(APTR psm, struct RastPort *rp, struct ColorMap *cm, ...);
+APTR ObtainDrawHandle(APTR psm, struct RastPort *rp, struct ColorMap *cm, Tag tag1, ...);
 APTR ObtainDrawHandleA(APTR psm, struct RastPort *rp, struct ColorMap *cm, struct TagItem *tags);
 
 void ReleaseDrawHandle(APTR drawhandle);
 
 
-BOOL DrawPicture(APTR drawhandle, APTR pic, UWORD x, UWORD y, ...);
+BOOL DrawPicture(APTR drawhandle, APTR pic, UWORD x, UWORD y, Tag tag1, ...);
 BOOL DrawPictureA(APTR drawhandle, APTR pic, UWORD x, UWORD y, struct TagItem *tags);
 
-BOOL MapPalette(APTR drawhandle, APTR palette, UBYTE *pentab, ...);
+BOOL MapPalette(APTR drawhandle, APTR palette, UBYTE *pentab, Tag tag1, ...);
 BOOL MapPaletteA(APTR drawhandle, APTR palette, UBYTE *pentab, struct TagItem *tags);
 
-LONG MapPen(APTR drawhandle, ULONG rgb, ...);
+LONG MapPen(APTR drawhandle, ULONG rgb, Tag tag1, ...);
 LONG MapPenA(APTR drawhandle, ULONG rgb, struct TagItem *tags);
 
-struct BitMap *CreatePictureBitMap(APTR drawhandle, APTR pic, ...);
+struct BitMap *CreatePictureBitMap(APTR drawhandle, APTR pic, Tag tag1, ...);
 struct BitMap *CreatePictureBitMapA(APTR drawhandle, APTR pic, struct TagItem *tags);
 
 
-ULONG DoPictureMethod(APTR pic, ULONG method, ...);
+ULONG DoPictureMethod(APTR pic, ULONG method, ULONG arg1, ...);
 ULONG DoPictureMethodA(APTR pic, ULONG method, ULONG *arguments);
 
-ULONG GetPictureAttrs(APTR pic, ...);
+ULONG GetPictureAttrs(APTR pic, Tag tag1, ...);
 ULONG GetPictureAttrsA(APTR pic, struct TagItem *tags);
 
-ULONG LockPicture(APTR pic, ULONG mode, ...);
+ULONG LockPicture(APTR pic, ULONG mode, ULONG arg1, ...);
 ULONG LockPictureA(APTR pic, ULONG mode, ULONG *arguments);
 void UnLockPicture(APTR pic, ULONG mode);
 
-BOOL IsPicture(char *filename, ...);
+BOOL IsPicture(char *filename, Tag tag1, ...);
 BOOL IsPictureA(char *filename, struct TagItem *tags);
 
-APTR CreateDirectDrawHandle(APTR drawhandle, UWORD sw, UWORD sh, UWORD dw, UWORD dh, ...);
+APTR CreateDirectDrawHandle(APTR drawhandle, UWORD sw, UWORD sh, UWORD dw, UWORD dh, Tag tag1, ...);
 APTR CreateDirectDrawHandleA(APTR drawhandle, UWORD sw, UWORD sh, UWORD dw, UWORD dh, struct TagItem *tags);
 void DeleteDirectDrawHandle(APTR ddh);
-BOOL DirectDrawTrueColor(APTR ddh, ULONG *array, UWORD x, UWORD y, ...);
+BOOL DirectDrawTrueColor(APTR ddh, ULONG *array, UWORD x, UWORD y, Tag tag1, ...);
 BOOL DirectDrawTrueColorA(APTR ddh, ULONG *array, UWORD x, UWORD y, struct TagItem *tags);
 
-BOOL CreatePictureMask(APTR pic, UBYTE *array, UWORD arraywidth, ...);
+BOOL CreatePictureMask(APTR pic, UBYTE *array, UWORD arraywidth, Tag tag1, ...);
 BOOL CreatePictureMaskA(APTR pic, UBYTE *array, UWORD arraywidth, struct TagItem *tags);
 
 #endif
