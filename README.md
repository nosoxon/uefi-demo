# UEFI / EDK II Graphics Demo

My first UEFI application built with [EDK II](https://github.com/tianocore/edk2).
Experiments with the UEFI Graphics Output Protocol. Generates the display below.

<center><img src=boxes.png></center>


## Running

I use [uefi-run](https://github.com/Richard-W/uefi-run) to quickly test EFI binaries. It
handles `qemu` invocation and autoruns the built application.

``` sh
$ make
$ uefi-run -b /usr/share/edk2/x64/OVMF.fd build/boxer.efi
```

## More Links

* [Graphics Output Protocol](https://uefi.org/specs/UEFI/2.10/12_Protocols_Console_Support.html#efi-graphics-output-protocol) &ndash; UEFI 2.10 Specification
* [UEFI 2.10 Specification](https://uefi.org/sites/default/files/resources/UEFI_Spec_2_10_Aug29.pdf)
