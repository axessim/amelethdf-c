module ah5_util_m
  use, intrinsic :: ISO_C_BINDING

  use h5lt

  implicit none
  !=============================== header =====================================!
  private ! all private

  ! The HDF5 file id
  public hid_t
  ! The HDF5 size type
  public hsize_t

  public ah5_c_string_ptr_to_f_string

  !================================ type ======================================!


  !================================ interfaces ================================!
  interface
     function strlen_c(str) &
          bind(C, name="strlen") &
          result(length)
       import c_ptr, c_size_t
       type(c_ptr), value :: str
       integer(c_size_t) :: length
     end function strlen_c

     subroutine strncpy_c(dest, src, n) bind(C)
       import c_char, c_size_t
       character(kind=c_char), intent(out) :: dest(*)
       character(kind=c_char), intent(in)  :: src(*)
       integer(c_size_t), value, intent(in) :: n
    end subroutine strncpy_c
  end interface

contains
  subroutine ah5_c_string_ptr_to_f_string(C_string, F_string)
    type(C_PTR), intent(in) :: C_string
    character(len=:), intent(inout), allocatable :: F_string

    character(len=1, kind=C_CHAR), dimension(:), pointer :: p_chars
    integer :: i

    if (allocated(F_string)) then
       deallocate(F_string)
    end if

    if (C_associated(C_string)) then
       allocate(character(strlen_c(C_string)) :: F_string)

       call C_F_pointer(C_string, p_chars, [len(F_string)+1])
       do i = 1, len(F_string)
          if (p_chars(i) == C_NULL_CHAR) exit
          F_string(i:i) = p_chars(i)
       end do
    end if
  end subroutine ah5_c_string_ptr_to_f_string

end module ah5_util_m
