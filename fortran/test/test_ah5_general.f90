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
    integer(hid_t) :: file_id = 0
    character(len=*), parameter :: filename = "test_creat_open_close.h5"
    character(len=:), allocatable :: entrypoint, name


    call testu_start_logical_group("test creat open close")!-------------------->

    call testu_assert_false(file_exists(filename), "file not exists")
    call ah5_create(filename, H5F_ACC_RDWR_F, file_id)
    call testu_assert_ah5err("create file")
    call testu_assert(file_id.gt.0, "open file id")
    call testu_assert_true(file_exists(filename), "The file is created")
    call remove_file(filename)

    call testu_assert_false(file_exists(filename), "file not exists")
    call ah5_create(filename, H5F_ACC_RDWR_F, file_id, "my entry point")
    call testu_assert_ah5err("create file")
    call testu_assert_true(file_exists(filename), "The file is created")

    call ah5_close(file_id)
    call testu_assert_ah5err("close file")

    file_id = 0
    call ah5_open(filename, H5F_ACC_RDWR_F, file_id)
    call testu_assert(file_id.gt.0, "open file id")
    call testu_assert_ah5err("open file")

    ! call ah5_read_entrypoint(file_id, entrypoint)
    ! call testu_assert_equal(entrypoint, "my entry point", "Check entry point value")

    call remove_file(filename)

    call testu_end_logical_group()!---------------------------------------------<


    call testu_start_logical_group("test path")!-------------------------------->

    call ah5_path_basename("/path/to/ah5_obj", name)
    call testu_assert_equal(name, 'ah5_obj', 'check basename')
    call ah5_path_basename("ah5_obj", name)
    call testu_assert_equal(name, 'ah5_obj', 'check basename')

    call testu_end_logical_group()!---------------------------------------------<
  end subroutine test_creat_open_close

end program test_ah5_genertal
