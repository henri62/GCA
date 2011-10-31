
public class IRBitStream {

  /**
   * @param args
   */
  public static void main(String[] args) {
    int addr = 4733;
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

}
