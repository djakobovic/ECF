AC_INIT([libecf], [1.6], [domagoj.jakobovic@fer.hr])
AM_INIT_AUTOMAKE

AC_PROG_CXX
LT_INIT

AC_MSG_CHECKING(if --with-mpi option is specified)
AC_ARG_WITH(mpi, [  --with-mpi=DIR          specify MPI directory],
        [mpi_dir=$withval])
AC_MSG_RESULT(${mpi_dir-"default"})

if test "x$mpi_dir" != "x"; then
        LIBS="$LIBS -L${mpi_dir}/lib:${mpi_dir}/lib64"
        CPPFLAGS="-I${mpi_dir}/include -D_MPI $CPPFLAGS"
fi
AM_CONDITIONAL(MPI, test "x$mpi_dir" != "x")

AC_CONFIG_FILES([Makefile ECF/Makefile])
AC_OUTPUT
