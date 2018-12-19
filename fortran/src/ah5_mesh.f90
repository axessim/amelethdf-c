module ah5_mesh_m
  use, intrinsic :: ISO_C_BINDING

  use ah5_error_m
  use ah5_util_m

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

  public ah5_mesh_t
  public ah5_axis_t
  public ah5_smesh_t
  !public ah5_meshx_t
  public ah5_msh_group_t
  public ah5_msh_instance_t
  public ah5_read_mesh
  public ah5_free_mesh

  public ah5_mesh_groups
  public ah5_msh_group_instances
  public ah5_msh_instance_path
  public ah5_msh_group_t_path
  !================================ type ======================================!

  ! The valid Amelet-HDF unstructured elements type
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
  ! Geometrical primitive
  character, public, parameter :: AH5_UELE_PLAN     = achar(15)
  character, public, parameter :: AH5_UELE_CIRCLE   = achar(16)
  character, public, parameter :: AH5_UELE_ELLIPSE  = achar(17)
  character, public, parameter :: AH5_UELE_CYLINDRE = achar(105)
  character, public, parameter :: AH5_UELE_CONE     = achar(106)
  character, public, parameter :: AH5_UELE_SPHERE   = achar(107)

  ! The valide Amelet-HDF group type
  integer, public, parameter :: AH5_GROUP_NODE = 1
  integer, public, parameter :: AH5_GROUP_EDGE = 2
  integer, public, parameter :: AH5_GROUP_FACE = 3
  integer, public, parameter :: AH5_GROUP_VOLUME = 4


  integer, public, parameter :: MSH_INVALID             = -1
  integer, public, parameter :: MSH_STRUCTURED          = 1
  integer, public, parameter :: MSH_UNSTRUCTURED        = 2

  ! The Amelet-HDF mesh group of group (set of groups).
  type, bind(C) :: ah5_groupgroup_t
     type(c_ptr)                     :: path
     integer(hsize_t)                :: nb_groupgroupnames
     type(c_ptr)                     :: groupgroupnames
  end type ah5_groupgroup_t

  ! The Amelet-HDF unstructured mesh group (set of elements).
  type, bind(C) :: ah5_ugroup_t
     type(c_ptr)                     :: path
     integer(c_int)                  :: entitytype
     integer(hsize_t)                :: nb_groupelts
     type(c_ptr)                     :: groupelts
  end type ah5_ugroup_t

  ! The Amelet-HDF unstructured mesh.
  type, bind(C) :: ah5_umesh_t
     integer(hsize_t)                :: nb_elementnodes
     type(c_ptr)                     :: elementnodes
     integer(hsize_t)                :: nb_elementtypes
     type(c_ptr)                     :: elementtypes
     integer(hsize_t), dimension(2)  :: nb_nodes
     type(c_ptr)                     :: nodes
     integer(hsize_t)                :: nb_groups
     type(c_ptr)                     :: groups
     integer(hsize_t)                :: nb_groupgroups
     type(c_ptr)                     :: groupgroups
     integer(hsize_t)                :: nb_som_tables
     type(c_ptr)                     :: som_tables
  end type ah5_umesh_t


  type, bind(C) :: ah5_axis_t
     integer(hsize_t) :: nb_nodes
     type(c_ptr) :: nodes
  end type ah5_axis_t

  type, bind(C) :: ah5_smesh_t
     type(AH5_axis_t) :: x
     type(AH5_axis_t) :: y
     type(AH5_axis_t) :: z
     integer(hsize_t) :: nb_groups
     type(c_ptr) :: groups
     integer(hsize_t) :: nb_groupgroups
     type(c_ptr) :: groupgroups 
     integer(hsize_t) ::nb_some_tables
     type(c_ptr) :: som_tables
  end type ah5_smesh_t

!!$  type, bind(C) :: ah5_meshx_t
!!$     type(AH5_smesh_t) ::    structured
!!$     type(AH5_umesh_t) ::    unstructured
!!$  end type ah5_meshx_t

  type, bind(C) :: ah5_msh_instance_t
     type(c_ptr) :: path
     integer(c_int) :: type
     type(AH5_umesh_t) ::   data
  end type ah5_msh_instance_t

  type, bind(C) :: ah5_msh_group_t
     type(c_ptr) :: path
     integer(hsize_t) :: nb_msh_instances
     type(c_ptr) :: msh_instances
     integer(hsize_t) ::   nb_mlk_instances
     type(c_ptr) :: mlk_instances
  end type ah5_msh_group_t

  ! AHDF mesh
  type, bind(C) :: ah5_mesh_t
     integer(hsize_t) :: nb_groups
     type(c_ptr) :: groups !=> ah5_msh_group_t
  end type ah5_mesh_t

  !================================ interfaces ================================!

  ! Binding with Amelet-HDF C library
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

     function AH5_read_mesh_c(file_id, mesh) &
          bind(C, name="AH5_read_mesh")
       import c_char, c_int, c_ptr
       integer(c_int), value, intent(in) :: file_id
       type(c_ptr), value :: mesh
       integer(kind=c_int) ::  ah5_read_mesh_c
     end function AH5_read_mesh_c

     subroutine ah5_free_mesh_c(mesh) &
          bind(C, name="AH5_free_mesh")
       import c_ptr
       type(c_ptr), value :: mesh
     end subroutine ah5_free_mesh_c

  end interface


contains
  ! read from file the /mesh
  subroutine ah5_read_mesh(file_id, mesh, hdferr)
    integer(hid_t), intent(in) :: file_id    ! File identifier
    type(ah5_mesh_t), target :: mesh       ! the output mesh
    integer,  optional :: hdferr ! Error code 0 on success
    ! and -1 on failure
    character(kind=c_char) :: errcof
    integer(c_int) :: errcoff
    integer :: fi
    character(len=25) :: ci

    errcoff =ah5_read_mesh_c(file_id , c_loc(mesh))
    fi=errcoff
    write(ci,*) fi    
    errcof=trim(ci)//c_null_char
    if (present(hdferr)) then
       call ah5_error_if(errcof, hdferr)
    else
       call ah5_error_if(errcof)
    end if
  end subroutine ah5_read_mesh

  function ah5_mesh_groups(this) result(meshgroups)
    type(ah5_mesh_t), intent(in) :: this
    type(ah5_msh_group_t), dimension(:), pointer :: meshgroups

    call C_F_POINTER(&
         CPTR=this%groups, &
         FPTR=meshgroups, &
         SHAPE=(/this%nb_groups/))
  end function ah5_mesh_groups

  function ah5_msh_group_instances(this) result(msh_instances)
    type(ah5_msh_group_t), intent(in) :: this
    type(ah5_msh_instance_t), dimension(:), pointer :: msh_instances

    call C_F_POINTER(&
         CPTR=this%msh_instances, &
         FPTR=msh_instances, &
         SHAPE=(/this%nb_msh_instances/))
  end function ah5_msh_group_instances

  ! read the msh_instance path
  subroutine ah5_msh_instance_path(this, path)
    type(ah5_msh_instance_t), intent(in) :: this
    character(len=:), intent(inout), allocatable :: path

    call ah5_c_string_ptr_to_f_string(this%path, path)
  end subroutine ah5_msh_instance_path

  ! read the msh_group_t path
  subroutine ah5_msh_group_t_path(this, path)
    type(ah5_msh_group_t), intent(in) :: this
    character(len=:), intent(inout), allocatable :: path

    call ah5_c_string_ptr_to_f_string(this%path, path)
  end subroutine ah5_msh_group_t_path

  ! release  mesh
  subroutine ah5_free_mesh(mesh, hdferr)
    type(ah5_mesh_t), target :: mesh       ! the output mesh
    integer, intent(out), optional :: hdferr ! Error code 0 on success
    ! and -1 on failure

    call ah5_free_mesh_c(c_loc(mesh))
  end subroutine ah5_free_mesh

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
  ! The indices in offset starts at 1 unlike native Amelet-HDF indexing.
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
         SHAPE=(/this%nb_nodes(2),this%nb_nodes(1)/))
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
