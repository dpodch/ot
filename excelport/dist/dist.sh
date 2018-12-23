#!/bin/sh

# имя пакета
SPECNAME=excelport
SRC_DIR="../src"

ERROR ()
{
	echo "$*"
	exit 1
}


# собирает пакеты уcтановки
make_rpm()
{
	pushd `pwd`
# выдёргиваем версию пакета
	VERSION_MAJOR=`cat $SPECNAME.spec | grep "define" | grep -E "\s+version_major\s+" | awk '{print $3}'`
	VERSION_MINOR=`cat $SPECNAME.spec | grep "define" | grep -E "\s+version_minor\s+" | awk '{print $3}'`
	VERSION_PATCH=`cat $SPECNAME.spec | grep "define" | grep -E "\s+version_patch\s+" | awk '{print $3}'`
	VERSION_BUILD=`cat $SPECNAME.spec | grep "define" | grep -E "\s+version_build\s+" | awk '{print $3}'`

	VERSION=$VERSION_MAJOR.$VERSION_MINOR.$VERSION_PATCH
	VERSION_FULL="$VERSION-$VERSION_BUILD"

	DSTDIR=/var/tmp/${SPECNAME}  #  временная директория с архивом файлов пакета
	PROGRAM=${SPECNAME}-${VERSION} # название пакета - имя и версия

	ARCHIVE="${DSTDIR}/${PROGRAM}.tgz" # название архива файлов пакета

# очистка рабочих каталогов
	rm -Rf $DSTDIR ||  ERROR "Cannot clean $DSTDIR"
	mkdir -p $DSTDIR/$PROGRAM/src/ || ERROR "Cannot mkdir src"
	mkdir -p $DSTDIR/$PROGRAM/dist || ERROR "Cannot mkdir dist"

# создание архива с файлами пакета и передача его rpm
	echo $SPECNAME $VERSION_FULL copy ...

# spec file
	cp -f ./$SPECNAME.spec $DSTDIR/${PROGRAM}

# исходники 
	mkdir -p	$DSTDIR/${PROGRAM}/src
	cd $SRC_DIR
	cp -fr	* $DSTDIR/${PROGRAM}/src/

	echo $SPECNAME $VERSION-$VERSION_BUILD copy OK

	echo $SPECNAME $VERSION-$VERSION_BUILD build ...
# сборка
	( cd $DSTDIR 
	  tar cfz $ARCHIVE ${PROGRAM} || ERROR "tar" )

	rpmbuild -tb $ARCHIVE --clean $BUILDFLAGS || ERROR "rpmbuild"

	echo $SPECNAME $VERSION-$VERSION_BUILD build OK

	popd

# очистка рабочих каталогов
	rm -Rf $DSTDIR
}

make_rpm

#
# vim: ts=4
#