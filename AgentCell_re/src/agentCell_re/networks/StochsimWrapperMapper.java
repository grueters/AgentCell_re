/**
 * AgentCell Cell.java
 *
 * AgentCell is a multi-scale agent-based platform for bacterial chemotaxis.
 *
 * @author Thierry Emonet and Michael J. North
 */
/*
 * Created on January 19, 2006
 *
 * 
 * 
 */
package agentCell_re.networks;

class StochsimWrapperMapper {
	public final static native long create1(String jarg1, int jarg2);
    
    public final static native long create2(String jarg1, String jarg2, int jarg3);

	public final static native void destroy(long jarg1);

	public final static native void finalise(long jarg1);

	public final static native void step(long jarg1, double jarg2);

	public final static native double getTime(long jarg1);

    public final static native double getTimeInc(long jarg1);

	// the getCopyNumber provided by StochSim did not work with
	// multi-complexes molecules. 
	public final static native int getCopynumber(long jarg1,
			String jarg2);

	// Tom Shimizu wrote getOVValue which works for both simple 
	// and multistate complex types, but requires a conjugate 
	// output variable with the name specified by the "arg0" 
	// argument to be specified in the INI files
	public final static native int getOVValue(long jarg1,
			String jarg2);

	public final static native void setDynamicValue(long jarg1,
			String jarg2, double jarg3);

	public final static native void ErrOut(long jarg1, String jarg2,
			int jarg3);

	public final static native void StatusOut(long jarg1, String jarg2);
}
