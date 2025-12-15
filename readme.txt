Title:

    onchipmem.resource

Description:

   The onchipmem.resource provides access to the On Chip Memory (OCM) 
   found on the APM460Ex SoC (used on the Sam460ex board for example).

   The OCM buffer is 64KB in size, 128-bit bus width, and is much faster 
   than main RAM, up to double the DDR2 speed, basically it has a speed 
   similar to the 460Ex L2 cache.

   It's main use is to store often accessed application data, or as a way 
   to trasfer more quicly a data buffer to/from video ram for example.

How to use:

    * copy onchipmem.resource.kmod into your kickstart directory

    * add the following line in the Kicklayout file:

    MODULE Kickstart/onchipmem.resource.kmod

    * copy the SDK directory into SDK: (if installed)

    * check the SDK directory for include files, autodoc and example

History:

24-Feb-2013 - Version 53.1

- Initial version for Sam460ex board

