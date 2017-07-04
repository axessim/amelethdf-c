module ah5_error_m

  implicit none
  !=============================== header =====================================!
  private ! all private

  public ah5_check
  public ah5_error
  public ah5_error_if

  public ah5err

  !================================ type ======================================!
  ! error flag
  integer :: ah5err

  !================================ interfaces ================================!

contains
  ! Check if an error occure
  subroutine ah5_check(message, must_stop)
    character(len=*) :: message                 ! the printed message
    logical, intent(in), optional :: must_stop
    logical :: must_stop1

    must_stop1 = .true.
    if (present(must_stop)) must_stop1 = must_stop

    if (ah5err < 0) then
       print *, message
       if (must_stop1) then
          stop
       endif
    endif
  end subroutine ah5_check

  ! Add an error (internal)
  subroutine ah5_error_if(test, hdferr, errcode)
    logical, intent(in) :: test
    integer, intent(out), optional :: hdferr
    integer, intent(in), optional :: errcode

    if (test) then
       if (present(errcode)) then
          ah5err = errcode
       else
          ah5err = -1
       end if

       if (present(hdferr)) then
          hdferr = ah5err
       end if
    end if
  end subroutine ah5_error_if

  ! Add an error (internal)
  subroutine ah5_error(message, must_stop, errcode)
    integer, intent(in), optional :: errcode
    character(len=*), intent(in), optional :: message
    logical, intent(in), optional :: must_stop

    if (present(errcode)) then
       ah5err = errcode
    else
       ah5err = -1
    end if

    if (present(message)) then
       call ah5_check(message, must_stop)
    end if
  end subroutine ah5_error
end module ah5_error_m
