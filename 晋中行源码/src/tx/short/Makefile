SHELL	=	sh

CC		=	cc $(Q64) -g
#CC		=	xlc $(Q64) 
LD		=	ld

RM		=	@rm -f
MV		=	@mv
CP		=	@cp
ECHO	=	@echo


# paths and flags
 
BIN	= $(HOME)/bin
LIBDIR  = $(HOME)/lib 

INC	= -I$(HOME)/src/incl_c -I$(HOME)/src/incl_ec -I$(HOME)/src/incl_pub
CLIB	= -L$(HOME)/lib -L.
CFLAGS	= $(INC) $(CLIB)
#LIBAPL  = -lbaltcp -lcomm -lbase -lcom -lpubdb -lpubdb_m -lc -lm
LIBAPL  = -lbaltcp -lcomm -lbase -lcom -lpubdb -lln -lcif -lpubdb_m -lcard -lc -lm -lhv -lC -llv

OBJS	= sp0110.so \
sp0900.so \
sp9622.so \
sp2295.so \
sp5839.so \
sp9121.so \
sp6610.so \
sp6000.so \
sp6700.so \
sp6707.so \
sp6708.so \
sp8930.so \
sp9202.so \
sp9203.so \
sp9205.so \
sp9300.so \
sp9317.so \
sp9400.so \
sp9401.so \
sp9402.so \
sp9403.so \
sp9404.so \
sp9405.so \
sp9406.so \
sp9407.so \
sp9408.so \
sp9505.so \
sp9549.so \
sp9550.so \
sp9552.so \
sp9553.so \
sp9555.so \
sp9559.so \
sp9560.so \
sp9561.so \
sp9562.so \
sp9563.so \
sp9564.so \
sp9565.so \
sp9566.so \
sp9567.so \
sp9568.so \
sp9569.so \
sp9570.so \
sp9580.so \
sp9581.so \
sp9598.so \
sp9599.so \
sp9630.so \
sp9633.so \
sp9663.so \
sp9664.so \
sp9665.so \
sp9666.so \
sp9667.so \
sp9668.so \
sp9668.so \
sp9669.so \
sp9670.so \
sp9671.so \
sp9672.so \
sp9673.so \
sp9683.so \
sp9685.so \
sp9686.so \
sp9690.so \
sp9691.so \
sp9692.so \
sp9693.so \
sp9694.so \
sp9695.so \
sp9696.so \
sp9700.so \
sp9701.so \
sp9702.so \
sp9703.so \
sp9704.so \
sp9705.so \
sp9706.so \
sp9707.so \
sp9708.so \
sp9709.so \
sp9710.so \
sp9711.so \
sp9712.so \
sp9713.so \
sp9714.so \
sp9715.so \
sp9716.so \
sp9717.so \
sp9718.so \
sp9719.so \
sp9720.so \
sp9721.so \
sp9722.so \
sp9723.so \
sp9724.so \
sp9725.so \
sp9726.so \
sp9727.so \
sp9728.so \
sp9729.so \
sp9730.so \
sp9731.so \
sp9732.so \
sp9733.so \
sp9734.so \
sp9735.so \
sp9736.so \
sp9737.so \
sp9738.so \
sp9739.so \
sp9740.so \
sp9741.so \
sp9742.so \
sp9743.so \
sp9744.so \
sp9745.so \
sp9746.so \
sp9747.so \
sp9748.so \
sp9749.so \
sp9750.so \
sp9751.so \
sp9752.so \
sp9753.so \
sp9754.so \
sp9755.so \
sp9756.so \
sp9757.so \
sp9758.so \
sp9759.so \
sp9760.so \
sp9761.so \
sp9762.so \
sp9763.so \
sp9764.so \
sp9765.so \
sp9766.so \
sp9767.so \
sp9768.so \
sp9769.so \
sp9770.so \
sp9771.so \
sp9772.so \
sp9773.so \
sp9774.so \
sp9775.so \
sp9776.so \
sp9777.so \
sp9778.so \
sp9779.so \
sp9780.so \
sp9781.so \
sp9782.so \
sp9783.so \
sp9784.so \
sp9785.so \
sp9786.so \
sp9787.so \
sp9788.so \
sp9789.so \
sp9790.so \
sp9791.so \
sp9792.so \
sp9793.so \
sp9794.so \
sp9795.so \
sp9796.so \
sp9797.so \
sp9798.so \
sp9799.so \
sp9800.so \
sp9801.so \
sp9802.so \
sp9803.so \
sp9804.so \
sp9805.so \
sp9806.so \
sp9807.so \
sp9808.so \
sp9809.so \
sp9810.so \
sp9811.so \
sp9812.so \
sp9813.so \
sp9814.so \
sp9815.so \
sp9816.so \
sp9817.so \
sp9818.so \
sp9819.so \
sp9820.so \
sp9821.so \
sp9822.so \
sp9823.so \
sp9824.so \
sp9825.so \
sp9826.so \
sp9827.so \
sp9828.so \
sp9829.so \
sp9830.so \
sp9831.so \
sp9832.so \
sp9833.so \
sp9834.so \
sp9835.so \
sp9836.so \
sp9837.so \
sp9838.so \
sp9839.so \
sp9840.so \
sp9841.so \
sp9842.so \
sp9843.so \
sp9844.so \
sp9845.so \
sp9846.so \
sp9847.so \
sp9848.so \
sp9849.so \
sp9850.so \
sp9851.so \
sp9852.so \
sp9853.so \
sp9854.so \
sp9855.so \
sp9856.so \
sp9857.so \
sp9858.so \
sp9859.so \
sp9860.so \
sp9861.so \
sp9862.so \
sp9863.so \
sp9864.so \
sp9865.so \
sp9866.so \
sp9867.so \
sp9868.so \
sp9869.so \
sp9870.so \
sp9871.so \
sp9872.so \
sp9873.so \
sp9874.so \
sp9875.so \
sp9876.so \
sp9877.so \
sp9878.so \
sp9879.so \
sp9880.so \
sp9881.so \
sp9882.so \
sp9883.so \
sp9884.so \
sp9885.so \
sp9886.so \
sp9887.so \
sp9888.so \
sp9889.so \
sp9890.so \
sp9891.so \
sp9892.so \
sp9893.so \
sp9894.so \
sp9895.so \
sp9896.so \
sp9897.so \
sp9898.so \
sp9899.so \
sp9900.so \
sp9901.so \
sp9902.so \
sp9903.so \
sp9904.so \
sp9905.so \
sp9906.so \
sp9907.so \
sp9908.so \
sp9909.so \
sp9910.so \
sp9911.so \
sp9912.so \
sp9913.so \
sp9914.so \
sp9915.so \
sp9916.so \
sp9917.so \
sp9918.so \
sp9919.so \
sp9920.so \
sp9921.so \
sp9922.so \
sp9923.so \
sp9924.so \
sp9925.so \
sp9926.so \
sp9927.so \
sp9928.so \
sp9929.so \
sp9930.so \
sp9931.so \
sp9932.so \
sp9933.so \
sp9934.so \
sp9935.so \
sp9936.so \
sp9937.so \
sp9938.so \
sp9940.so \
sp9941.so \
sp9942.so \
sp9943.so \
sp9944.so \
sp9945.so \
sp9946.so \
sp9947.so \
sp9948.so \
sp9949.so \
sp9950.so \
sp9951.so \
sp9952.so \
sp9953.so \
sp9954.so \
sp9955.so \
sp9956.so \
sp9957.so \
sp9958.so \
sp9959.so \
sp9960.so \
sp9961.so \
sp9962.so \
sp9963.so \
sp9964.so \
sp9965.so \
sp9966.so \
sp9967.so \
sp9968.so \
sp9969.so \
sp9970.so \
sp9971.so \
sp9972.so \
sp9973.so \
sp9975.so \
sp9976.so \
sp9977.so \
sp9978.so \
sp9979.so \
sp9980.so \
sp9981.so \
sp9982.so \
sp9983.so \
sp9984.so \
sp9985.so \
sp9986.so \
sp9988.so \
sp9989.so \
sp9990.so \
sp9991.so \
sp9993.so \
sp9994.so \
sp9995.so \
sp9996.so \
sp9998.so \
sp9999.so \
sp9661.so \
sp9662.so \
spTIPS.so \
sp9324.so \
sp9636.so \
sp3369.so \
sp5666.so \
sp5580.so \
sp9646.so \
sp1175.so \
sp1176.so \
sp9655.so \
sp9204.so \
sp3405.so \
sp9331.so \
sp3468.so \
sp8508.so \
sp9018.so \
sp9130.so \
sp8890.so \
sp8891.so \
sp8889.so \
sp9659.so \
sp7868.so \
sp9610.so \
sp9629.so \
sp9237.so \
sp9592.so \
sp8137.so \
spTCDZ.so \
sp8138.so \
sp9321.so \
sp9669.so \
sp9597.so \
sp8893.so \
sp9015.so \
sp9162.so \
sp6555.so \
sp8018.so \
sp0009.so \
sp3222.so \
sp9303.so \
sp6750.so \
sp1537.so \
sp5563.so \
sp4415.so \
sp4416.so \
sp9939.so \
sp9222.so \
sp9028.so \
sp1234.so \
sp8606.so \
sp8683.so \
sp8314.so \
sp8315.so \
sp8320.so \
sp9637.so \
sp8321.so
#sp9160.so 
#sp9161.so 
#sp9939.so ln_auth_mtr.date结构中不包含此项
#sp9870.so sp9992.so结构问题？

TARGET	= $(BIN)/lib.so

all: Mstart $(TARGET) Mcomplete
 
Mstart:
	$(ECHO)
	$(ECHO) "MAKE lib.so:$(TARGET) ..."
	$(ECHO)

Mcomplete:
	$(ECHO)
	$(ECHO) "MAKE lib.so:$(TARGET) completed."
	$(ECHO)
 
$(TARGET):$(OBJS)


# implicit
 
.SUFFIXES: .so .c
 
.c.so:
	$(CC) $(CFLAGS) -g -s -c $*.c
	$(CC) -G  -o $@ $(CFLAGS) $*.o $(LIBAPL)
	mv $(*F).so $(LIBDIR)
	$(RM) $(*F).o

.c:
	$(CC) -g -s -G  -o $@ $(CFLAGS) $*.c $(LIBAPL)
	mv *.so $(LIBDIR)

# default
 
#.DEFAULT:
 
# clearing object codes
 
clean: cleanup
	$(RM) $(TARGET)
 
cleanup:
	$(RM) $(OBJS)
 
# dependencies

