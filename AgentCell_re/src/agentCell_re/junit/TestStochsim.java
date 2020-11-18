/*
 * Created on Mar 31, 2006
 *
 * To change the template for this generated file go to
 * Window>Preferences>Java>Code Generation>Code and Comments
 */
package agentCell_re.junit;

import agentCell_re.molecules.Molecule;
import agentCell_re.networks.Stochsim;

public class TestStochsim {
    public static final String DIR = "/home/emonet/docs/workspace/agentcell/runs/CHER200";
    
    /**
     * @param args
     */
    public static void main(String[] args) {        
        
        Stochsim stochsim = new Stochsim("STCHSTC.INI",DIR + "/cell1/network1/Input");
        System.out.println("stochsim created");
        
        System.out.println("At t = " + stochsim.getTime() +":");
//        System.out.println("    dt = " + stochsim.getTimeInc());
        System.out.println("    CheYp = " + stochsim.getCopynumber(Molecule.CHEYP));

        stochsim.step(0.3);
        System.out.println("At t = " + stochsim.getTime() +":");
//        System.out.println("    dt = " + stochsim.getTimeInc());
        System.out.println("    CheYp = " + stochsim.getCopynumber(Molecule.CHEYP));

        System.out.println("Congrats, everything went well. Bye...");
    }

}
