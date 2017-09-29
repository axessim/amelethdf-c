!
! Provide a minimal unit test framwork for Frotran 90
!
#if !defined(AH5_DATA_DIR)
#error "Test need define AH5_DATA_DIR"
#endif

#if !defined(AH5_TEST_DATA_DIR)
#error "Test need define AH5_TEST_DATA_DIR"
#endif


module utest_m
  use ISO_C_BINDING
  use h5lt
  use ah5_error_m

  implicit none

  !================================ header ====================================!
  private ! all private

  public testu_start_logical_group
  public testu_end_logical_group

  public testu_exit

  public testu_assert
  public testu_assert_true
  public testu_assert_false
  public testu_assert_equal
  public testu_assert_size
  public testu_assert_equal_all
  public testu_assert_almost_equal

  public testu_assert_h5_attribute
  public testu_assert_ah5err

  public file_exists
  public remove_file


  !================================ parameters ================================!
  integer, parameter :: mkfloat = 4
  integer, parameter :: mkdouble = 8
  character(len=*), parameter :: ligne_slim =  '-------------------------------------------'
  character(len=*), parameter :: ligne_heavy = '==========================================='

  integer :: level = 1
  integer :: env = 0, nb_error = 0
  integer :: count = 1
  character(len=100), dimension(100) :: memo_name
  character(len=100) :: memo_msg


  interface testu_assert_equal
     module procedure &
          testu_assert_equal_f, &
          testu_assert_equal_d, &
          testu_assert_equal_i, &
          testu_assert_equal_s, &
          testu_assert_equal_b, &
          testu_assert_equal_ci
  end interface

  interface testu_assert_equal_all
     module procedure &
          testu_assert_equal_all_i,&
          testu_assert_equal_all_l3,&
          testu_assert_equal_all_l3_
  end interface testu_assert_equal_all

  interface
     function remove_c(filename) &
          bind(c, name="remove") &
          result(errno)
       import c_int, c_char
       character(c_char), dimension(*), intent(in) :: filename
       integer(c_int) :: errno
     end function remove_c
  end interface

contains
  subroutine testu_exit()
    if (nb_error.gt.0) then
       nb_error = nb_error / 0 ! raise an error
       stop "Test FAILD"
    end if
  end subroutine testu_exit


  subroutine testu_update_env(test, msg, errormsg, ce)
    logical, intent(in) :: test
    character(len=*), intent(in), optional :: msg
    character(len=*), intent(in), optional :: errormsg
    logical, intent(out), optional :: ce

    if (test) then
       if (present(msg).and.memo_msg.ne.msg) then
          print*, msg//' ... '//'OK'
       end if
    else
       if (present(msg)) then
          print*, ligne_slim
          print*, msg//' ... '//'FAIL', count + 1
          if (present(errormsg)) then
             print*, errormsg
          end if
          print*, ligne_slim
       end if
       env = env + 1
    end if

    !
    if (present(msg).and.memo_msg.ne.msg) then
       count = 1
       memo_msg = msg
    else if (present(msg).and.memo_msg.eq.msg) then
       count = count + 1
    end if


    if (present(ce)) then
       ce = test
    end if
  end subroutine testu_update_env


  subroutine testu_assert_ah5err(msg)
    character(len=*), intent(in), optional  :: msg

    call testu_assert(ah5err.eq.0, msg)
  end subroutine testu_assert_ah5err


  subroutine testu_assert_h5_attribute(file_id, path, attr, val, msg)
    integer(hid_t), intent(in) :: file_id
    character(len=*), intent(in) :: path
    ! the name of the attribute
    character(len=*), intent(in) :: attr
    ! the returned value
    character(len=*), intent(in) :: val
    character(len=*), intent(in), optional  :: msg

    logical :: f_corder_valid
    integer :: cset, corder
    integer(hsize_t) :: data_size

    character(len=len(val)+10) :: buf1
    logical :: here, mandatory1

    integer :: hdferr

    here = .false.
    buf1 = ""
    call h5aexists_by_name_f(file_id, path, attr, &
         here, hdferr, H5P_DEFAULT_F)

    call testu_assert(here, "Can't read attribute for "//path//"@"//attr)

    if (here) then
       !init data_size
       data_size = -1
       call h5aget_info_by_name_f(file_id,path, attr, f_corder_valid, &
            corder, cset, data_size, hdferr)
       call h5ltget_attribute_string_f(file_id, path, attr, &
            buf1, hdferr)
       call testu_assert(hdferr.lt.0, "Can't read attribute for "//path//"@"//attr)
       call trim_data_size_char(buf1, data_size)
       call trim_null_char(buf1)
       call testu_assert_equal(trim(buf1), val, msg)
    endif
  end subroutine testu_assert_h5_attribute


  elemental subroutine trim_null_char(string, c)
    character(len=*), intent(inout) :: string
    character, intent(in), optional :: c
    character :: c1
    integer :: ind

    c1 = ""
    if (present(c)) c1 = c

    ind = scan(string, char(0))
    if (ind/=0) string(ind:) = c1
  end subroutine trim_null_char


  elemental subroutine trim_data_size_char(string, data_size, c)
    character(len=*), intent(inout) :: string
    integer(hsize_t), intent(in) :: data_size
    character, intent(in), optional :: c
    character :: c1
    integer :: ind

    c1 = ""
    if (present(c)) c1 = c

    if ((data_size+1)/=0) string((data_size+1):) = c1
  end subroutine trim_data_size_char


  subroutine testu_assert_equal_f(v1, v2, msg, tol, ce)
    real(kind=mkfloat), intent(in) :: v1, v2
    character(len=*), intent(in), optional  :: msg
    real(kind=mkfloat), intent(in), optional :: tol
    logical, intent(out), optional :: ce

    real(kind=mkfloat) :: itol

    logical :: test

    character(len=100) :: errormsg
    write(errormsg, *) v1, '!=', v2

    if (present(tol)) then
       itol = tol
    else
       itol = 0._mkfloat
    end if

    test = abs(v1-v2).le.itol
    call testu_update_env(test, msg, errormsg=errormsg, ce=ce)
  end subroutine testu_assert_equal_f


  subroutine testu_assert_equal_d(v1, v2, msg, tol, ce)
    real(kind=mkdouble), intent(in) :: v1, v2
    character(len=*), intent(in), optional  :: msg
    real(kind=mkdouble), intent(in), optional :: tol
    logical, intent(out), optional :: ce

    real(kind=mkdouble) :: itol

    logical :: test

    character(len=100) :: errormsg
    write(errormsg, *) v1, '!=', v2

    if (present(tol)) then
       itol = tol
    else
       itol = 0._mkdouble
    end if

    test = abs(v1-v2).le.itol
    call testu_update_env(test, msg, errormsg=errormsg, ce=ce)
  end subroutine testu_assert_equal_d


  subroutine testu_assert_equal_b(v1, v2, msg, ce)
    logical, intent(in) :: v1, v2
    character(len=*), intent(in), optional  :: msg
    logical, intent(out), optional :: ce

    character(len=100) :: errormsg
    write(errormsg, *) v1, '!=', v2

    call testu_update_env(v1.eqv.v2, msg, errormsg=errormsg, ce=ce)
  end subroutine testu_assert_equal_b

  subroutine testu_assert_equal_i(v1, v2, msg, ce)
    integer, intent(in) :: v1, v2
    character(len=*), intent(in), optional  :: msg
    logical, intent(out), optional :: ce

    character(len=100) :: errormsg
    write(errormsg, *) v1, '!=', v2

    call testu_update_env(v1.eq.v2, msg, errormsg=errormsg, ce=ce)
  end subroutine testu_assert_equal_i

  subroutine testu_assert_equal_ci(v1, v2, msg, ce)
    integer(c_size_t), intent(in) :: v1, v2
    character(len=*), intent(in), optional  :: msg
    logical, intent(out), optional :: ce

    character(len=100) :: errormsg
    write(errormsg, *) v1, '!=', v2

    call testu_update_env(v1.eq.v2, msg, errormsg=errormsg, ce=ce)
  end subroutine testu_assert_equal_ci


  subroutine testu_assert_equal_s(v1, v2, msg, ce)
    character(len=*), intent(in) :: v1, v2
    character(len=*), intent(in), optional  :: msg
    logical, intent(out), optional :: ce

    character(len=len_trim(v1)+len_trim(v2)+3) :: errormsg
    write(errormsg, *) trim(v1), '!=', trim(v2)

    call testu_update_env(v1.eq.v2, msg, errormsg=errormsg, ce=ce)
  end subroutine testu_assert_equal_s


  subroutine testu_assert_size(v1, v2, msg, ce)
    integer, dimension(:), intent(in) :: v1, v2
    character(len=*), intent(in), optional  :: msg
    logical, intent(out), optional :: ce

    call testu_assert_equal_i(size(v1), size(v2), msg, ce=ce)
  end subroutine testu_assert_size


  subroutine testu_assert_equal_all_i(v1, v2, msg, ce)
    integer, dimension(:), intent(in) :: v1, v2
    character(len=*), intent(in), optional  :: msg
    logical, intent(out), optional :: ce

    character(len=1000) :: errormsg
    write(errormsg, *) v1, '!=', v2

    call testu_update_env(all(v1.eq.v2), msg, ce=ce, errormsg=errormsg)
  end subroutine testu_assert_equal_all_i

  subroutine testu_assert_equal_all_l3(v1, v2, msg, ce)
    logical, dimension(:,:,:), intent(in) :: v1, v2
    character(len=*), intent(in), optional  :: msg
    logical, intent(out), optional :: ce

    character(len=100) :: errormsg
    write(errormsg, *) v1, '!=', v2

    call testu_update_env(all(v1.eqv.v2), msg, ce=ce, errormsg=errormsg)
  end subroutine testu_assert_equal_all_l3

  subroutine testu_assert_equal_all_l3_(l, msg, ce)
    logical, dimension(:,:,:), intent(in) :: l
    character(len=*), intent(in), optional  :: msg
    logical, intent(out), optional :: ce

    character(len=100) :: errormsg
    write(errormsg, *) "not all true"

    call testu_update_env(all(l), msg, ce=ce, errormsg=errormsg)
  end subroutine testu_assert_equal_all_l3_

  subroutine testu_assert_almost_equal(actual, desired, decimal, msg, ce)
    real, dimension(:), intent(in) :: actual, desired
    integer, intent(in), optional :: decimal
    character(len=*), intent(in), optional  :: msg
    logical, intent(out), optional :: ce

    integer :: decimal_
    character(len=100) :: errormsg

    decimal_ = 6
    if (present(decimal)) &
         decimal_ = decimal

    write(errormsg, *) 'Arrays are not almost equal ', &
         maxval(abs(desired-actual)), " > ", 0.5 * 10.0**(-decimal_)

    call testu_update_env((maxval(abs(desired-actual)) .lt. 0.5 * 10.0**(-decimal_)), &
         msg, ce=ce, errormsg=errormsg)
  end subroutine testu_assert_almost_equal

  !
  subroutine testu_assert(test, msg, ce)
    logical, intent(in) :: test
    character(len=*), intent(in), optional  :: msg
    logical, intent(out), optional :: ce

    call testu_update_env(test, msg, ce=ce)
  end subroutine testu_assert

  !
  subroutine testu_assert_true(test, msg, ce)
    logical, intent(in) :: test
    character(len=*), intent(in), optional  :: msg
    logical, intent(out), optional :: ce

    call testu_update_env(test.eqv..true., msg, ce=ce)
  end subroutine testu_assert_true

  !
  subroutine testu_assert_false(test, msg, ce)
    logical, intent(in) :: test
    character(len=*), intent(in), optional  :: msg
    logical, intent(out), optional :: ce

    call testu_update_env(test.eqv..false., msg, ce=ce)
  end subroutine testu_assert_false

  !
  subroutine testu_start_logical_group(name)
    character(len=*), intent(in) :: name
    select case(level)
    case(1)
       print*, ligne_heavy
    case default
       print*, ''
       print*, name
    end select

    memo_name(level) = name
    level = level + 1

  end subroutine testu_start_logical_group

  !
  subroutine testu_end_logical_group(msg)
    character(len=*), intent(in), optional :: msg
    integer :: inter_level


    level = level - 1


    if (level.eq.1) then
       if (env.gt.0) then
          print*, ligne_slim
          print*, memo_name(level)
          print'("FAILED (failures=", i4, ")")', env
          print*, ligne_heavy
          nb_error = nb_error + env
          env = 0
       else
          print*, ligne_slim
          print*, memo_name(level)
          print*, "PAST"
          print*, ligne_heavy
       end if
    end if
  end subroutine testu_end_logical_group

  function file_exists(filename) result(res)
    character(len=*),intent(in) :: filename
    logical                     :: res

    ! Check if the file exists
    inquire(file=trim(filename), exist=res)
  end function file_exists

  subroutine remove_file(filename)
    character(len=*),intent(in) :: filename
    integer errno

    errno = remove_c(trim(filename)//c_null_char)
  end subroutine remove_file
end module utest_m
