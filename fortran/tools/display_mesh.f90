! This tool read a mesh in the provided file and
! displays its content.

program display_mesh
  use ah5_general_m
  use ah5_mesh_m
  use h5lt
  use h5g

  implicit none

  character(len=:), allocatable :: fpath  ! Input file path.
  character(len=:), allocatable :: npath  ! Input node path.

  integer(hid_t) :: ahdf = 0  ! File ID.
  integer :: status = 0  ! success: 0, error: -1
  integer :: ndisp = 5  ! Number of displayed stuff.
  integer :: idx, igrp  ! Indices for loops.

  type(ah5_umesh_t) :: mesh  ! Mesh to be displayed.
  real, dimension(:, :), pointer :: nodes  ! Coordinates of the nodes.
  integer, dimension(:), pointer :: enodes  ! Indices of the nodes of the
                                            ! elements.
  character, dimension(:), pointer :: etypes  ! Types of the elements.
  integer, dimension(:), allocatable :: offsets  ! Location in enodes of first
                                                 ! node index of each element.
  character :: etype  ! An element type.
  type(ah5_ugroup_t), dimension(:), pointer :: groups  ! Groups of elements.
  type(ah5_groupgroup_t), dimension(:), pointer :: groupgroups
  character(len=:), allocatable :: name

  ! Input file from command line.
  call input_file(fpath, npath)
  write(*, *) "Display this file ", fpath

  ! Open file.
  call ah5_open(fpath, H5F_ACC_RDONLY_F, ahdf)
  if (ahdf .lt. 0) then
     write(*, *) " > Fail to open input file."
     stop 1
  endif

  write(*, *) "Display this mesh: ", npath

  ! Display the meshes.
  call ah5_read_umesh(ahdf, npath, mesh, status)
  call check_status( &
       status, "Fail to read the provided path as an unstructured mesh.")
  ! Display the content of the mesh
  write(*, *) "Geometrical of the mesh:"

  ! Nodes
  nodes => ah5_umesh_nodes(mesh)
  write(*, *) "Nodes"
  write(*, *) "  | Number: ", mesh%nb_nodes(1)
  write(*, *) "  | ", min(mesh%nb_nodes(1), ndisp), " first nodes."
  do idx = 1, min(mesh%nb_nodes(1), ndisp)
     write(*, *) "  |", nodes(idx, 1), ", ", nodes(idx, 2), ", ", nodes(idx, 3)
  enddo

  ! Elements
  enodes => ah5_umesh_elementnodes(mesh)
  etypes => ah5_umesh_elementtypes(mesh)
  call ah5_umesh_elementoffsets(mesh, offsets)
  write(*, *) "Elements"
  write(*, *) "  | Number: ", mesh%nb_elementtypes
  write(*, *) "  | ", min(mesh%nb_elementtypes, ndisp), " first elements."
  do idx = 1, min(mesh%nb_elementtypes, ndisp)
     etype = etypes(idx)
     write(*, *) "  | Type: ", ichar(etype)
     ! Take care of the indexing :
     ! offset starts at one (allowing to do the following statement),
     write(*, *) "  | Node indices: ", &
          enodes(offsets(idx): offsets(idx + 1) - 1) + 1
     ! but indices stored in elementNodes starts at 0, so we add 1.
     write(*, *) "  | First node: ", nodes(enodes(offsets(idx)) + 1, :)
  enddo

  ! Groups
  ! Get the groups
  groups => ah5_umesh_groups(mesh)
  write(*, *) "Groups"
  write(*, *) "  | Number: ", mesh%nb_groups
  write(*, *) "  | ", min(mesh%nb_groups, ndisp), " first groups."
  do idx = 1, min(mesh%nb_groups, ndisp)
     call ah5_ugroup_path(groups(idx), npath)
     call ah5_path_basename(npath, name)
     enodes => ah5_ugroup_elements_index(groups(idx))
     write(*, *) "  | ", name, " with ", size(enodes), " elements: "!!!!!!!!!!!!!
     write(*, *) "  |   ", enodes(1: min(size(enodes), ndisp)), "..."
  enddo

  ! Group group
  groupgroups => ah5_umesh_groupggroups(mesh)
  write(*, *) "Groups of groups"
  write(*, *) "  | Number: ", mesh%nb_groupgroups
  write(*, *) "  | ", min(mesh%nb_groupgroups, ndisp), " first groups."
  do idx = 1, min(mesh%nb_groupgroups, ndisp)
     call ah5_groupgroup_path(groupgroups(idx), npath)
     call ah5_path_basename(npath, name)
     write(*, *) "  | ", name, " containing ", &
          groupgroups(idx)%nb_groupgroupnames, " groups:"
     do igrp = 1, groupgroups(idx)%nb_groupgroupnames
        call ah5_groupgroup_getitem(groupgroups(idx), igrp, name)
        write(*, *) "  |   ", name
     enddo
     ! ah5_groupgroup_getitem
  enddo


  ! Release the mesh.
  call ah5_free_umesh(mesh)

  ! Close the file.
  call ah5_close(ahdf)

contains
  subroutine input_file(fpath, npath)
    integer :: num_args, length
    character(len=:), allocatable :: message
    character(len=:), allocatable :: fpath ! Path of the input file
    character(len=:), allocatable :: npath ! Path of the mesh node

    num_args = command_argument_count()
    if (num_args == 2) then
       ! Input file.
       call get_command_argument(1, length=length)
       allocate(character(length) :: fpath)
       call get_command_argument(1, fpath)
       ! Mesh node
       call get_command_argument(2, length=length)
       allocate(character(length) :: npath)
       call get_command_argument(2, npath)
    else
       if (num_args == 0) then
          message = "No input file had been provided."
       else if (num_args == 1) then
          message = "The path of the mesh must be provided."
       else
          message = "Only two arguments are supported."
       endif
       write(*, *) "Error: ", message
       call usage()
       stop 1
    endif

  end subroutine input_file

  subroutine check_status(status, message)
    integer :: status
    character(len=*) :: message
    if (status /= 0) then
       write(*, *) " > ", message, " (status: ", status, ")"
       stop 1
    endif
  end subroutine check_status

  subroutine usage()
    write(*, *) "Usage: fortran_display_mesh file meshpath"
    write(*, *) "  file     : Path of the Amelet-HDF input file."
    write(*, *) "  meshpath : Path of the unstructured mesh to be displayed."
  end subroutine usage

end program display_mesh
