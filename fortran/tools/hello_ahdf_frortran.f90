! Hello world.

program hello
  use ah5_general_m
  use h5lt

  implicit none

  integer(hid_t) :: ahdf = 0

  print *, "Hello World!"
  call ah5_create("hello_fortran.h5", H5F_ACC_RDWR_F, ahdf)
  call ah5_close(ahdf)

end program hello
