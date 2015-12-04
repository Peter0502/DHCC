ch()
{
    echo "$str----->$str1"
    files=`grep $str *XML|grep -v ^1000.XML|awk -F ":" '{print $1}'`
    for file in $files
    do
        echo $file
        cmd=`sed -e "s/$str/$str1/g" $file > $file.tmp`
        mv $file.tmp $file
    done
}

str="请打印普通凭证"
str1="请打印业务专用凭证"
#ch

str="打印专用凭证"
str1="打印业务专用凭证"
#ch

right()
{
    for line in `cat menu.xml|sed -e "s/ /./g"|grep RIGHTS|grep ACTION|grep TITLE|grep ".XML"`
    do
        echo $line
        file=`echo $line|awk -F "\"" '{print $4}'`
        rights=`echo $line|awk -F "\"" '{print $2}'`

        echo "$file++++$rights"
        if [ -f $file ]
        then
            frights=`grep RIGHTS $file|awk -F "\"" '{print $2}'`
            echo "<<<<<<<<<<$frights"
            if [ $rights != $frights ]
            then
                echo "66666666666666666666666$frights<========>$rights"
                cmd="sed -e 's/RIGHTS=\"$frights/RIGHTS=\"$rights/g' $file>$file.tmp; mv $file.tmp $file; rm -rf $file.tmp"
                echo $cmd>cmd.sh
                chmod a+x cmd.sh
                ./cmd.sh
            fi
        fi
    done
}

right
