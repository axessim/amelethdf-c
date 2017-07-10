module ah5_general_m
  use, intrinsic :: ISO_C_BINDING

  use ah5_error_m

  use h5lt


  implicit none
  !=============================== header =====================================!
  private ! all private

  public hid_t
  public H5F_ACC_RDWR_F
  public H5F_ACC_RDONLY_F

  public ah5_create
  public ah5_open
  public ah5_close
  public ah5_read_entrypoint

  public ah5_path_basename

  !================================ type ======================================!


  !================================ interfaces ================================!
  interface
     ! Create a Amelet-HDF file and set entry point if not null.
     function ah5_create_c(name, flags, entry_point) &
          bind(c, name="AH5_create") &
          result(file_id)
       import c_int, c_char, c_ptr
       character(c_char), dimension(*), intent(in) :: name
       integer(c_int), value, intent(in) :: flags
       type(c_ptr), value, intent(in) :: entry_point
       integer(c_int) :: file_id
     end function ah5_create_c

     ! Open a Amelet-HDF file.
     function ah5_open_c(name, flags) &
          bind(c, name="AH5_open") &
          result(file_id)
       import c_int, c_char
       character(c_char), dimension(*), intent(in) :: name
       integer(c_int), value, intent(in) :: flags
       integer(c_int) :: file_id
     end function ah5_open_c

     ! Close a Amelet-HDF file
     function ah5_close_c(file_id) &
          bind(c, name="AH5_close") &
          result(status)
       import c_int
       integer(c_int), value, intent(in) :: file_id
       integer(c_int) :: status
     end function ah5_close_c

     ! Read the AmeletHDF entry point and copies into the array
     ! pointed by entrypoint, including the terminating null character
     ! (and stopping at that point).
     function ah5_read_entrypoint_c(file_id, entrypoint) &
          bind(c, name="AH5_read_entrypoint") &
          result(status)
       import c_int, c_char, c_ptr
       integer(c_int), value, intent(in) :: file_id
       type(c_ptr), value, intent(in) :: entrypoint
       type(c_ptr) :: status
     end function ah5_read_entrypoint_c

     ! Returns the C string length of the Amelet-HDF entry point.
     function ah5_read_entrypoint_strlen_c(file_id) &
          bind(c, name="AH5_read_entrypoint_strlen") &
          result(strlen)
       import c_int
       integer(c_int), value, intent(in) :: file_id
       integer(c_int) :: strlen
     end function ah5_read_entrypoint_strlen_c
  end interface

contains

  ! Create a Amelet-HDF file and set entry point if not nil.
  subroutine ah5_create(name, flags, file_id, entrypoint, hdferr)
    character(len=*), intent(in) :: name   ! Name of the file
    integer, intent(in) :: flags           ! File access flags
                                           ! Possible values are:
                                           ! H5F_ACC_RDWR_F
                                           ! H5F_ACC_RDONLY_F
    integer(hid_t), intent(out) :: file_id ! File identifier
    character(len=*), intent(in), optional :: entrypoint ! The
                                                         ! Amelet-HDF
                                                         ! entry point
                                                         ! if nil it
                                                         ! is ignored.
    integer, intent(out), optional :: hdferr ! Error code 0 on success
    ! and -1 on failure

    character(c_char), dimension(:), pointer :: entrypoint_

    ! set error to 0
    ah5err = 0

    if (present(entrypoint)) then
       allocate(entrypoint_(len(entrypoint)+1))
       entrypoint_ = trim(entrypoint)//c_null_char

       file_id = ah5_create_c(trim(name)//c_null_char, &
            flags, c_loc(entrypoint_))
    else
       file_id = ah5_create_c(trim(name)//c_null_char, &
            flags, c_null_ptr)
    end if

    call ah5_error_if(file_id.le.0, hdferr, file_id)
  end subroutine ah5_create

  ! Open a Amelet-HDF file
  subroutine ah5_open(name, flags, file_id, hdferr)
    character(len=*), intent(in) :: name   ! Name of the file
    integer, intent(in) :: flags           ! File access flags
                                           ! Possible values are:
                                           ! H5F_ACC_RDWR_F
                                           ! H5F_ACC_RDONLY_F
    integer(hid_t), intent(out) :: file_id ! File identifier
    integer, intent(out), optional :: hdferr ! Error code 0 on success
                                             ! and -1 on failure

    ! set error to 0
    ah5err = 0

    file_id = ah5_open_c(trim(name)//c_null_char, flags)

    call ah5_error_if(file_id.le.0, hdferr, file_id)
  end subroutine ah5_open

  ! Close a Amelet-HDF file
  subroutine ah5_close(file_id, hdferr)
    integer(hid_t), intent(in) :: file_id    ! File identifier
    integer, intent(out), optional :: hdferr ! Error code 0 on success
                                             ! and -1 on failure

    integer :: errco
    errco = ah5_close_c(file_id)
    call ah5_error_if(errco.gt.0, hdferr, errco)
  end subroutine ah5_close

  ! Read the AmeletHDF entry point
  subroutine ah5_read_entrypoint(file_id, entrypoint, hdferr)
    integer(hid_t), intent(in) :: file_id    ! File identifier
    character(len=:), allocatable, intent(out) :: entrypoint ! The
                                                             ! Amelet-HDF
                                                             ! entry
                                                             ! point
    integer, intent(out), optional :: hdferr ! Error code 0 on success
                                             ! and -1 on failure

    integer :: strlen
    character(c_char), dimension(:), pointer :: buf
    type(c_ptr) :: status


    ! strlen = AH5_read_entrypoint_strlen_c(file_id)
    ! allocate(character(len=strlen) :: entrypoint)
    ! allocate(buf(strlen+1))
    !
    ! status = AH5_read_entrypoint_c(file_id, c_loc(buf))
    ! call ah5_error_if(c_associated(status), hdferr)
    !
    ! print*, buf
  end subroutine ah5_read_entrypoint

  ! get the basename ot the given path
  subroutine ah5_path_basename(path, name)
    character(len=*), intent(in) :: path
    character(len=:), intent(inout), allocatable :: name

    integer :: sep_idx

    sep_idx = index(path, '/', back=.true.)

    if (allocated(name)) then
       deallocate(name)
    end if

    allocate(character(len(path) - sep_idx - 1) :: name)
    name = path(sep_idx+1:)
  end subroutine ah5_path_basename

end module ah5_general_m
