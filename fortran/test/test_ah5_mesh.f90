program test_ah5_genertal
  use ah5_general_m
  use ah5_mesh_m
  use utest_m
  use h5lt

  implicit none


  call testu_start_logical_group('test mesh')!=================================>
  call test_read_umesh
  call testu_end_logical_group()!==============================================<
  call testu_exit


contains
  subroutine test_read_umesh
    integer(hid_t) :: file_id
    character(len=*), parameter :: filename = &
         AH5_DATA_DIR // "/ah5_1_5_4_generic_mesh.h5"
    character(len=*), parameter :: path = &
         "/mesh/genericmesh/unstructuredMesh"

    type(ah5_umesh_t) :: umesh
    type(ah5_ugroup_t), dimension(:), pointer :: groups
    type(ah5_groupgroup_t), dimension(:), pointer :: groupgroups
    integer, dimension(:), allocatable :: offset
    character(len=:), allocatable :: name

    call testu_start_logical_group("test read umesh")!-------------------------->
    call ah5_open(filename, H5F_ACC_RDONLY_F, file_id)
    call testu_assert_ah5err("open file")

    call ah5_read_umesh(file_id, path, umesh)
    call testu_assert_ah5err("read umesh")

    call testu_assert(umesh%nb_elementnodes.eq.17, 'nb elementnodes')
    call testu_assert(all(ah5_umesh_elementnodes(umesh) .eq. &
         (/0, 1, 1, 3, 5, 0, 1, 3, 4, 0, 1, 3, 4, 9, 0, 1, 3/)), 'element nodes')

    call testu_assert(umesh%nb_elementtypes.eq.5, 'nb element type')
    call testu_assert(all(ah5_umesh_elementtypes(umesh) .eq. &
         (/AH5_UELE_BAR2, AH5_UELE_TRI3, AH5_UELE_QUAD4, &
         AH5_UELE_TETRA4, AH5_UELE_PLAN/)), 'element types')

    call testu_assert(all(umesh%nb_nodes.eq.(/12, 3/)), 'nb nodes')
    call testu_assert_almost_equal(reshape(ah5_umesh_nodes(umesh), (/36/)), (/ &
         0.1, 0.0, 0.0, &
         0.2, 0.0, 0.0, &
         0.3, 0.0, 0.0, &
         0.0, 0.1, 0.0, &
         0.0, 0.2, 0.0, &
         0.0, 0.3, 0.0, &
         0.0, 0.0, 0.1, &
         0.0, 0.0, 0.2, &
         0.0, 0.0, 0.3, &
         0.1, 0.1, 0.1, &
         0.2, 0.2, 0.2, &
         0.3, 0.3, 0.3 /))

    ! offset
    call ah5_umesh_elementoffsets(umesh, offset)
    call testu_assert_equal_all(offset, (/1, 3, 6, 10, 14, 17/), 'effset')

    ! groups
    call testu_assert(umesh%nb_groups.eq.6, 'nb group')
    groups => ah5_umesh_groups(umesh)
    call testu_assert_equal(size(groups), 6, 'nb group')

    ! check group 1
    call testu_assert(groups(1)%nb_groupelts.eq.1, 'groups(1)nb_groupelts')
    call ah5_ugroup_path(groups(1), name)
    call testu_assert_equal(&
         name, '/mesh/genericmesh/unstructuredMesh/group/grpbar2', &
         'group%name')
    call testu_assert_equal(&
         groups(1)%entitytype, AH5_GROUP_EDGE, 'groups(1)group%entitytype')
    call testu_assert(all(ah5_ugroup_elements_index(groups(1)) .eq. &
         (/0/)), 'ugroup elements index')

    ! check group 2
    call testu_assert(groups(2)%nb_groupelts.eq.2, 'groups(2)nb_groupelts')
    call ah5_ugroup_path(groups(2), name)
    call testu_assert_equal(&
         name, '/mesh/genericmesh/unstructuredMesh/group/grpnode', &
         'group%name')
    call testu_assert_equal(&
         groups(2)%entitytype, AH5_GROUP_NODE, 'groups(2)group%entitytype')
    call testu_assert(all(ah5_ugroup_elements_index(groups(2)) .eq. &
         (/1, 5/)), 'ugroup elements index')

    ! check group 3
    call testu_assert(groups(3)%nb_groupelts.eq.1, 'groups(3)nb_groupelts')
    call ah5_ugroup_path(groups(3), name)
    call testu_assert_equal(&
         name, '/mesh/genericmesh/unstructuredMesh/group/grpplane', &
         'group%name')
    call testu_assert_equal(&
         groups(3)%entitytype, AH5_GROUP_FACE, 'groups(3)group%entitytype')
    call testu_assert(all(ah5_ugroup_elements_index(groups(3)) .eq. &
         (/4/)), 'ugroup elements index')

    ! check group 4
    call testu_assert(groups(4)%nb_groupelts.eq.1, 'groups(4)nb_groupelts')
    call ah5_ugroup_path(groups(4), name)
    call testu_assert_equal(&
         name, '/mesh/genericmesh/unstructuredMesh/group/grpquad4', &
         'group%name')
    call testu_assert_equal(&
         groups(4)%entitytype, AH5_GROUP_FACE, 'groups(4)group%entitytype')
    call testu_assert(all(ah5_ugroup_elements_index(groups(4)) .eq. &
         (/2/)), 'ugroup elements index')

    ! check group 5
    call testu_assert(groups(5)%nb_groupelts.eq.1, 'groups(5)nb_groupelts')
    call ah5_ugroup_path(groups(5), name)
    call testu_assert_equal(&
         name, '/mesh/genericmesh/unstructuredMesh/group/grptetra4', &
         'group%name')
    call testu_assert_equal(&
         groups(5)%entitytype, AH5_GROUP_VOLUME, 'groups(5)group%entitytype')
    call testu_assert(all(ah5_ugroup_elements_index(groups(5)) .eq. &
         (/3/)), 'ugroup elements index')

    ! check group 6
    call testu_assert(groups(6)%nb_groupelts.eq.1, 'groups(6)nb_groupelts')
    call ah5_ugroup_path(groups(6), name)
    call testu_assert_equal(&
         name, '/mesh/genericmesh/unstructuredMesh/group/grptri3', &
         'group%name')
    call testu_assert_equal(&
         groups(6)%entitytype, AH5_GROUP_FACE, 'groups(6)group%entitytype')
    call testu_assert(all(ah5_ugroup_elements_index(groups(6)) .eq. &
         (/1/)), 'ugroup elements index')

    ! group group
    call testu_assert(umesh%nb_groupgroups.eq.1, 'nb group group')
    groupgroups => ah5_umesh_groupggroups(umesh)
    call testu_assert_equal(size(groupgroups), 1, 'nb group group')

    call ah5_groupgroup_path(groupgroups(1), name)
    call testu_assert_equal(&
         name, '/mesh/genericmesh/unstructuredMesh/groupGroup/grpgrpface', &
         'group%name')

    call testu_assert(groupgroups(1)%nb_groupgroupnames.eq.2, 'nb_groupgroupnames')
    call ah5_groupgroup_getitem(groupgroups(1), 1, name)
    call testu_assert_equal(name, 'grptri3', 'groupgroups(1)%groupgroupnames(1)')
    call ah5_groupgroup_getitem(groupgroups(1), 2, name)
    call testu_assert_equal(name, 'grpquad4', 'groupgroups(1)%groupgroupnames(2)')

    call ah5_free_umesh(umesh)
    call testu_assert_ah5err("free umesh")

    call ah5_close(file_id)
    call testu_assert_ah5err("close file")

    deallocate(name)

    call testu_end_logical_group()!---------------------------------------------<
  end subroutine test_read_umesh

end program test_ah5_genertal
