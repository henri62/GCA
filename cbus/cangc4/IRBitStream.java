
public class IRBitStream {
    static int addr = 4733;
    static byte[] ir = new byte[4];
    static byte f40khz    = 1;
    static byte modBitCnt = 0;
    static byte bitStatus = 0;
    static byte bitIndex  = 0;

    public static void main(String[] args) {
        int idx = 0;
        int cycle = 0;
      makeBitStream();
      do {
          System.out.print(getBitStatus());
          // delay 12µs
          try{Thread.sleep(12);} catch (InterruptedException e){}
          System.out.print(" ");
          cycle++;
          if( modBitCnt == 0 ) {
            System.out.print(" ("+bitIndex+")");
            idx++;
            /*
            if( idx == 32 ) {
              idx = 0; // recycle
              System.out.print(" ");
            }
            */
          }
          // simulate interrupt
          try{Thread.sleep(12);} catch (InterruptedException e){}
    	  
      }while(bitIndex < 34);
    }
  /**
   * @param args
   */
  public static void main2(String[] args) {
    byte[] bits = new byte[16];
    byte[] ir = new byte[17*2];
    byte p1, p0;
    
    //---------- BIT stream
    // d13 ... d0 p1 p0
    // address
    for( int i = 0; i < 14; i++ ) {
      bits[i] = (byte)((addr & (1 << 13-i)) > 0 ? 1:0);
    }
    
    // parity bit 1
    p1 = bits[0];
    for( int i = 1; i < 14; i+=2 ) {
      p1 = (byte)(p1 ^ bits[i]);
    }
    bits[14] = p1;

    // parity bit 0
    p0 = bits[1];
    for( int i = 2; i < 14; i+=2 ) {
      p0 = (byte)(p0 ^ bits[i]);
    }
    bits[15] = p0;
    
    //---------- IR stream
    // 11=sync, 00=0, 01=1
    // sync
    ir[0] = 1;
    ir[1] = 1;
    
    for( int i = 0; i < 16; i++ ) {
      if( bits[i] == 0 ) {
        ir[i*2+2] = 0;
        ir[i*2+3] = 0;
      }
      else {
        ir[i*2+2] = 0;
        ir[i*2+3] = 1;
      }
    }
    
    int idx = 0;
    int cycle = 0;
    while(true) {
      System.out.print(ir[idx]);
      // delay 12µs
      try{Thread.sleep(12);} catch (InterruptedException e){}
      System.out.print(" ");
      cycle++;
      if( cycle == 10 ) {
        cycle = 0;
        System.out.print("\n");
        idx++;
        if( idx == 17*2 ) {
          idx = 0; // recycle
          System.out.print(" ");
        }
      }
      // simulate interrupt
      try{Thread.sleep(12);} catch (InterruptedException e){}
    }
    
    
    
  }
  
  
  static void makeBitStream() {
	  int bits;
	  byte p1;
	  byte p0;
	  byte i;

	  //---------- BIT stream
	  // d13 ... d0 p1 p0
	  // address
	  bits = addr & 0x3FFF;

	  // parity bit 1
	  p1 = (byte)(bits & 0x0001);
	  for( i = 1; i < 14; i+=2 ) {
	    p1 = (byte)(p1 ^ (bits >> i & 0x0001));
	  }
	  bits |= (p1&1) << 14;

	  // parity bit 0
	  p0 = (byte)(bits & 0x0001);
	  for( i = 2; i < 14; i+=2 ) {
	    p0 = (byte)(p0 ^ (bits>>i & 0x0001));
	  }
	  bits |= (p1&1) << 15;

	  //---------- IR stream
	  // 11=sync, 00=0, 01=1
	  // sync
	  ir[0] = 0;
	  ir[1] = 0;
	  ir[2] = 0;
	  ir[3] = 0;

	  for( i = 0; i < 16; i++ ) {
		  byte b = (byte)((bits >> i) & 0x0001);
		  System.out.println("i="+i+" b="+b);
	    if( ((bits >> i) & 0x0001) == 1 ) {
	      ir[(i*2)/8] |= 1 << ((i*2)%8+1);
	    }
	  }
      System.out.println("bitstream " + ir[0] + " " + ir[1] + " " + ir[2] + " " + ir[3] + " bits " + bits);

	}

  static char getBitStatus() {
	  modBitCnt++;
	  if( modBitCnt >= 20 ) {

	    bitStatus = 0;
	    if( bitIndex < 2 ) {
	      bitStatus = 1;
	      System.out.print("\nSYNC i="+bitIndex+" s="+bitStatus+" ");
	    }
	    else {
	    	int byteIdx = (bitIndex-2) / 8;
	    	int mask = (1 << (bitIndex-2) % 8);
	      if( (ir[(bitIndex-2) / 8] & (1 << (bitIndex-2) % 8)) > 0 )
	        bitStatus = 1;
	      System.out.print("\nADDR i="+bitIndex+" s="+bitStatus+" bi="+byteIdx+" mask="+mask+" ");
	    }

	    modBitCnt = 0;
	    bitIndex++;
	    if( bitIndex >= 34)
	      bitIndex = 0;
	  }
	  
	  if( bitStatus == 1)// && f40khz == 1 )
	    return '1';
	  else
	    return '0';
	}



}
