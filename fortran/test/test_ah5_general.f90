
program test_ah5_genertal
  use ah5_general_m
  use utest_m
  use h5lt

  implicit none

  call testu_start_logical_group('test ah5 general')!==========================>
  call test_creat_open_close
  call testu_end_logical_group()!==============================================<
  call testu_exit



contains
  subroutine test_creat_open_close
    integer(hid_t) :: file_id
    character(len=*), parameter :: filename = "test_creat_open_close.h5"
    character(len=:), allocatable :: entrypoint

    call testu_start_logical_group("test creat open close")!-------------------->

    call testu_assert_false(file_exists(filename), "file not exists")
    call ah5_create(filename, H5F_ACC_RDWR_F, file_id)
    call testu_assert_true(file_exists(filename), "The file is created")
    call remove_file(filename)

    call testu_assert_false(file_exists(filename), "file not exists")
    call ah5_create(filename, H5F_ACC_RDWR_F, file_id, "my entry point")
    call testu_assert_true(file_exists(filename), "The file is created")

    call ah5_close(file_id)

    call ah5_open(filename, H5F_ACC_RDWR_F, file_id)

    ! call ah5_read_entrypoint(file_id, entrypoint)
    ! call testu_assert_equal(entrypoint, "my entry point", "Check entry point value")

    call remove_file(filename)

    call testu_end_logical_group()!---------------------------------------------<
  end subroutine test_creat_open_close

end program test_ah5_genertal
