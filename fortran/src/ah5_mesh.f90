module ah5_mesh_m
  use, intrinsic :: ISO_C_BINDING

  use ah5_error_m
  use ah5_util_m

  use h5lt


  implicit none
  !=============================== header =====================================!
  private ! all private

  public ah5_umesh_t
  public ah5_read_umesh
  public ah5_free_umesh
  public ah5_umesh_elementnodes
  public ah5_umesh_elementtypes
  public ah5_umesh_elementoffsets
  public ah5_umesh_nodes
  public ah5_umesh_groups
  public ah5_umesh_groupggroups

  public ah5_ugroup_t
  public ah5_ugroup_path
  public ah5_ugroup_elements_index

  public ah5_groupgroup_t
  public ah5_groupgroup_path
  public ah5_groupgroup_getitem

  !================================ type ======================================!
  integer, parameter :: c_hsize_t = c_size_t

  character, public, parameter :: AH5_UELE_BAR2     = achar(1)
  character, public, parameter :: AH5_UELE_BAR3     = achar(2)
  character, public, parameter :: AH5_UELE_TRI3     = achar(11)
  character, public, parameter :: AH5_UELE_TRI6     = achar(12)
  character, public, parameter :: AH5_UELE_QUAD4    = achar(13)
  character, public, parameter :: AH5_UELE_QUAD8    = achar(14)
  character, public, parameter :: AH5_UELE_QUAD9    = achar(18)
  character, public, parameter :: AH5_UELE_TETRA4   = achar(101)
  character, public, parameter :: AH5_UELE_PYRA5    = achar(102)
  character, public, parameter :: AH5_UELE_PENTA6   = achar(103)
  character, public, parameter :: AH5_UELE_HEXA8    = achar(104)
  character, public, parameter :: AH5_UELE_TETRA10  = achar(108)
  character, public, parameter :: AH5_UELE_HEXA20   = achar(109)

  character, public, parameter :: AH5_UELE_PLAN     = achar(15)
  character, public, parameter :: AH5_UELE_CIRCLE   = achar(16)
  character, public, parameter :: AH5_UELE_ELLIPSE  = achar(17)
  character, public, parameter :: AH5_UELE_CYLINDRE = achar(105)
  character, public, parameter :: AH5_UELE_CONE     = achar(106)
  character, public, parameter :: AH5_UELE_SPHERE   = achar(107)

  integer, public, parameter :: AH5_GROUP_NODE = 1
  integer, public, parameter :: AH5_GROUP_EDGE = 2
  integer, public, parameter :: AH5_GROUP_FACE = 3
  integer, public, parameter :: AH5_GROUP_VOLUME = 4



  type, bind(C) :: ah5_groupgroup_t
     type(c_ptr)                     :: path
     integer(c_hsize_t)              :: nb_groupgroupnames
     type(c_ptr)                     :: groupgroupnames
  end type ah5_groupgroup_t

  type, bind(C) :: ah5_ugroup_t
     type(c_ptr)                     :: path
     integer(c_int)                  :: entitytype
     integer(c_hsize_t)              :: nb_groupelts
     type(c_ptr)                     :: groupelts
  end type ah5_ugroup_t

  type, bind(C) :: ah5_umesh_t
     integer(c_hsize_t)              :: nb_elementnodes
     type(c_ptr)                     :: elementnodes
     integer(c_hsize_t)              :: nb_elementtypes
     type(c_ptr)                     :: elementtypes
     integer(c_hsize_t), dimension(2):: nb_nodes
     type(c_ptr)                     :: nodes
     integer(c_hsize_t)              :: nb_groups
     type(c_ptr)                     :: groups
     integer(c_hsize_t)              :: nb_groupgroups
     type(c_ptr)                     :: groupgroups
     integer(c_hsize_t)              :: nb_som_tables
     type(c_ptr)                     :: som_tables
  end type ah5_umesh_t

  !================================ interfaces ================================!

  interface
     function ah5_read_umesh_c(file_id, path, umesh) &
          bind(C, name="AH5_read_umesh") &
          result(errco)
       import c_int, c_char, c_ptr
       integer(c_int), value, intent(in) :: file_id
       character(c_char), dimension(*), intent(in) :: path
       type(c_ptr), value :: umesh
       character(c_char) :: errco
     end function ah5_read_umesh_c

     subroutine ah5_free_umesh_c(umesh) &
          bind(C, name="AH5_free_umesh")
       import c_ptr
       type(c_ptr), value :: umesh
     end subroutine ah5_free_umesh_c

     function ah5_element_size_c(elementtype) &
          bind(C, name="AH5_element_size") &
          result(element_size)
       import c_char, c_int
       character(c_char), intent(in), value :: elementtype
       integer(c_int) :: element_size
     end function ah5_element_size_c
  end interface


contains
  ! read from file the unstructured mesh
  subroutine ah5_read_umesh(file_id, path, umesh, hdferr)
    integer(hid_t), intent(in) :: file_id    ! File identifier
    character(len=*), intent(in) :: path     ! the mesh path read
    type(ah5_umesh_t), target :: umesh       ! the output mesh
    integer, intent(out), optional :: hdferr ! Error code 0 on success
                                             ! and -1 on failure

    character(c_char) :: errco

    errco = ah5_read_umesh_c(file_id, trim(path)//c_null_char, c_loc(umesh))

    call ah5_error_if(errco, hdferr)
  end subroutine ah5_read_umesh

  ! release unstrucrtured mesh
  subroutine ah5_free_umesh(umesh, hdferr)
    type(ah5_umesh_t), target :: umesh       ! the output mesh
    integer, intent(out), optional :: hdferr ! Error code 0 on success
                                             ! and -1 on failure

    call ah5_free_umesh_c(c_loc(umesh))
  end subroutine ah5_free_umesh

  ! return the elementnodes
  function ah5_umesh_elementnodes(this) result(elementnodes)
    type(ah5_umesh_t), intent(in) :: this

    integer(c_int), dimension(:), pointer :: elementnodes

    call C_F_POINTER(&
         CPTR=this%elementnodes, &
         FPTR=elementnodes, &
         SHAPE=(/this%nb_elementnodes/))
  end function ah5_umesh_elementnodes

  ! return the elementtypes
  function ah5_umesh_elementtypes(this) result(elementtypes)
    type(ah5_umesh_t), intent(in) :: this

    character(c_char), dimension(:), pointer :: elementtypes

    call C_F_POINTER(&
         CPTR=this%elementtypes, &
         FPTR=elementtypes, &
         SHAPE=(/this%nb_elementtypes/))
  end function ah5_umesh_elementtypes

  ! build element offset. The element nodes index are
  ! 'elementnodes(offset(element_index):offset(element_index+1)-1)'
  subroutine ah5_umesh_elementoffsets(this, offset)
    type(ah5_umesh_t), intent(in) :: this
    integer, intent(inout), dimension(:), allocatable :: offset

    character(c_char), dimension(:), pointer :: elementtypes
    integer :: i

    if (allocated(offset)) then
       if (size(offset).ne.(this%nb_elementtypes+1)) then
          deallocate(offset)
          allocate(offset(this%nb_elementtypes+1))
       end if
    else
       allocate(offset(this%nb_elementtypes+1))
    end if

    elementtypes => ah5_umesh_elementtypes(this)

    offset(1) = 1
    do i=1, this%nb_elementtypes
       offset(i+1) = offset(i) + AH5_element_size_c(elementtypes(i))
    end do
  end subroutine ah5_umesh_elementoffsets

  ! return the nodes
  function ah5_umesh_nodes(this) result(nodes)
    type(ah5_umesh_t), intent(in) :: this

    real(c_float), dimension(:,:), pointer :: nodes

    call C_F_POINTER(&
         CPTR=this%nodes, &
         FPTR=nodes, &
         SHAPE=this%nb_nodes)
  end function ah5_umesh_nodes

  ! return groups
  function ah5_umesh_groups(this) result(groups)
    type(ah5_umesh_t), intent(in) :: this

    type(ah5_ugroup_t), dimension(:), pointer :: groups

    call C_F_POINTER(&
         CPTR=this%groups, &
         FPTR=groups, &
         SHAPE=(/this%nb_groups/))
  end function ah5_umesh_groups

  ! read the group path
  subroutine ah5_ugroup_path(this, path)
    type(ah5_ugroup_t), intent(in) :: this
    character(len=:), intent(inout), allocatable :: path

    call ah5_c_string_ptr_to_f_string(this%path, path)
  end subroutine ah5_ugroup_path

  ! return the element index
  function ah5_ugroup_elements_index(this) result(elements)
    type(ah5_ugroup_t), intent(in) :: this

    integer(c_int), dimension(:), pointer :: elements

    call C_F_POINTER(&
         CPTR=this%groupelts, &
         FPTR=elements, &
         SHAPE=(/this%nb_groupelts/))
  end function ah5_ugroup_elements_index

  ! return groupggroups
  function ah5_umesh_groupggroups(this) result(groupgroups)
    type(ah5_umesh_t), intent(in) :: this

    type(ah5_groupgroup_t), dimension(:), pointer :: groupgroups

    call C_F_POINTER(&
         CPTR=this%groupgroups, &
         FPTR=groupgroups, &
         SHAPE=(/this%nb_groupgroups/))
  end function ah5_umesh_groupggroups

  ! read the group path
  subroutine ah5_groupgroup_path(this, path)
    type(ah5_groupgroup_t), intent(in) :: this
    character(len=:), intent(inout), allocatable :: path

    call ah5_c_string_ptr_to_f_string(this%path, path)
  end subroutine ah5_groupgroup_path

  ! return the idx group name
  subroutine ah5_groupgroup_getitem(this, idx, name)
    type(ah5_groupgroup_t), intent(in) :: this
    integer, intent(in) :: idx
    character(len=:), intent(inout), allocatable :: name

    type(c_ptr), dimension(:), pointer :: groupgroupnames

    call C_F_POINTER(&
         CPTR=this%groupgroupnames, &
         FPTR=groupgroupnames, &
         SHAPE=(/this%nb_groupgroupnames/))

    call ah5_c_string_ptr_to_f_string(groupgroupnames(idx), name)
  end subroutine ah5_groupgroup_getitem

end module ah5_mesh_m
