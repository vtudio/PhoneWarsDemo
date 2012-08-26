# Automatically copy all common texture resources into Android folder at compile time

# First check that the directories exist, and if not, create them
if [ ! -d "res/drawable" ]; then
	mkdir res/drawable
else
    rm -rf res/drawable/*
fi

if [ ! -d "res/raw" ]; then
	mkdir res/raw
else
    rm -rf res/raw/*
fi

# Then copy the files
cp -r icon.png res/raw/
cp -r ../../Engine/Resources/ res/raw/
cp -r ../../App/Resources/ res/raw/

# Now we need to convert all filenames containing uppercase to all lowercase
# (which seems to be a stupid Android requirement)
function toLowerCase
{
dir=`pwd`
for x in `find .`
    do
	if [ ! -f $x ]; then
		continue
	fi

	subdirectory="../raw";
	extension=`echo "$x"|awk -F . '{print $NF}'`

	if [ $extension == "png" -o $extension == "jpg" ]; then
	    subdirectory="../drawable";
	fi
	
	if [ $extension == "mp4" ]; then
	    subdirectory="../../assets";
	fi

	lowercase=`echo $x  | tr '[A-Z]' '[a-z]'`
	stripped=`basename $lowercase`
	mv $x "$dir/$subdirectory/$stripped"
    
	done
}

cd res/raw
toLowerCase
cd ..
