ERROR ()
{
	echo ERROR: $*
	exit 1
}

#$1 - directory with build script
#$2 - script to run, dist.sh by default
build_dir ()
{
	DIR=$1
	echo "build_dir ${DIR}"

	[ -d $DIR ] || ERROR "$DIR not found"

	pushd `pwd`
	echo "pushd >" `pwd`

	cd $DIR || ERROR "$DIR not found"

	echo "CD ${DIR}"

	source ./dist.sh || ERROR "Building $DIR"
	echo $SPECNAME
	echo $VERSION
	echo $VERSION_FULL

	if [ "$SPECNAME" = "" ] ; then
		SPECNAME=$NPR
	fi
	
	if [ "$VERSION" = "" ] ; then
		VERSION=$VERSIONSTR
	fi

	popd
	echo "popd >" `pwd`
}

# get latest RPM, filter by $1 (name)
get_latest_rpm ()
{
	FILTER=${1:-rpm}
	FILES=`find ~/rpmbuild/RPMS | grep "/$FILTER-[0-9]"`

    if [ "$FILES" = "" ] ; then
        echo ""
        return
    else
        ls -t $FILES | head --lines=1
    fi
}

# Копирование rpm'ки
# $1 - базовая часть названия пакета
# $2 - конфигурация в которую копировать
copy_rpm ()
{
	cp -v `get_latest_rpm $1` $2 || ERROR "Cannot copy $1 RPM to $REPO_DIR/$2"
}

REPO_DIR="repo"

rm -rf ${REPO_DIR}
mkdir  ${REPO_DIR}

build_dir "../dist"
copy_rpm excelport ${REPO_DIR}
cp ../dist/deps/served/el7/libserved-1.4.3-DS1.x86_64.rpm ${REPO_DIR}
