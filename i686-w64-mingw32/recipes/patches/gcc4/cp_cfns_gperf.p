--- gcc/cp/cfns.gperf.orig	2017-09-15 16:17:49.249145794 +0100
+++ gcc/cp/cfns.gperf	2017-09-15 16:18:26.468005263 +0100
@@ -16,14 +16,6 @@
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
 %}
 %%
 # The standard C library functions, for feeding to gperf; the result is used
