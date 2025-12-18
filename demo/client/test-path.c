/**
 * Essaie des chargements de LDG dans sous-répertoires
 */
#include <stdio.h>
#include <gem.h>
#include <ldg.h>


void load_lib( char *name) {
  LDG *lib;

  lib = ldg_open( name, ldg_global);
  if( lib == NULL)
    printf( "Chargement de %s echoue\n", name);
  else {
    printf( "Chargement de %s reussi\n", name);
    ldg_close( lib, ldg_global);
  }
}

int main( void) {
  LDG *lib ;

  appl_init();

  load_lib( "mem.ldg");
  load_lib( "userdef.ldg");
  load_lib( "windom\\userdef.ldg");  
  load_lib( "windom/userdef.ldg");  
  
  load_lib( "C:/gemsys/ldg/mem.ldg");
  load_lib( "/c/gemsys/ldg/mem.ldg");

  appl_exit();
}
