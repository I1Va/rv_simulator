MEMORY {
  SNIPPY (rwx) : ORIGIN = 4096, LENGTH = 131072
}
SECTIONS {
  .snippy.text.rx 4096: {
  KEEP(*(.snippy.text.rx))
} >SNIPPY
  .snippy.data.rw 69632 (NOLOAD) : {
  KEEP(*(.snippy.data.rw))
} >SNIPPY
}