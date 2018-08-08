--- gcc/cp/cfns.h.orig	2017-09-15 16:50:55.872127506 +0100
+++ gcc/cp/cfns.h	2017-09-15 16:51:18.147443810 +0100
@@ -47,14 +47,6 @@
 You should have received a copy of the GNU General Public License
 along with GCC; see the file COPYING3.  If not see
 <http://www.gnu.org/licenses/>.  */
-#ifdef __GNUC__
-__inline
-#endif
-static unsigned int hash (const char *, unsigned int);
-#ifdef __GNUC__
-__inline
-#endif
-const char * libc_name_p (const char *, unsigned int);
 /* maximum key range = 391, duplicates = 0 */
 
 #ifdef __GNUC__
