# ExternalProject ���� ��ɾ� �� �߰�
include(ExternalProject)

# Dependency ���� ���� ����
set(DEP_INSTALL_DIR ${PROJECT_BINARY_DIR}/install)
set(DEP_INCLUDE_DIR ${DEP_INSTALL_DIR}/include)
set(DEP_LIB_DIR ${DEP_INSTALL_DIR}/lib)

# DirectXTex
ExternalProject_Add(
    dep_DirectXTex
    GIT_REPOSITORY "https://github.com/microsoft/DirectXTex.git"
    GIT_TAG "sept2023"
    GIT_SHALLOW 1
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR}
    TEST_COMMAND ""
)
set(DEP_LIST ${DEP_LIST} dep_DirectXTex)
set(DEP_LIBS ${DEP_LIBS} DirectXTex)