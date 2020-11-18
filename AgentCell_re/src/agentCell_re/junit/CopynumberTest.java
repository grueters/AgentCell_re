/**
 * AgentCell Cell.java
 *
 * AgentCell is a multi-scale agent-based platform for bacterial chemotaxis.
 *
 * @author Thierry Emonet and Michael J. North
 */
/*

AgentCell is an agent-based simulation of bacterial chemotaxis.
Copyright (c) 2005 Trustees of the University of Chicago

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

Authors: Thierry Emonet (emonet@uchicago.edu) and Michael J. North (north@anl.gov)

	Thierry Emonet
	920 E. 58th Street
	University of Chicago
	Chicago, IL 60637

	Michael J. North
	9700 S. Cass Avenue
	Argonne, IL 60439

*/
/*
 * Created on Jun 9, 2004
 *
 * 
 * 
 */
package agentCell_re.junit;

import agentCell_re.molecules.Concentration;
import agentCell_re.molecules.Copynumber;
import agentCell_re.molecules.Molecule;
import junit.framework.TestCase;


/**
 * @author emonet
 *
 * 
 * 
 */
public class CopynumberTest extends TestCase {
    /**
     * Constructor for CopynumberTest.
     * @param arg0
     */
    public CopynumberTest(String arg0) {
        super(arg0);
    }

    public static void main(String args[]) {
        junit.swingui.TestRunner.run(CopynumberTest.class);
    }

    /*
     * @see TestCase#setUp()
     */
    protected void setUp() throws Exception {
        super.setUp();
    }

    /*
     * @see TestCase#tearDown()
     */
    protected void tearDown() throws Exception {
        super.tearDown();
    }

    /*
     * Test for void Copynumber(String)
     */
    final public void testCopynumberString() {
        Copynumber cheYp = new Copynumber(Molecule.CHEYP);
        TestCase.assertEquals(0, cheYp.getLevel());
    }

    /*
     * Test for void Copynumber(String, long)
     */
    final public void testCopynumberStringlong() {
        Copynumber cheYp = new Copynumber(Molecule.CHEYP, 12);
        TestCase.assertEquals(12, cheYp.getLevel());
        TestCase.assertEquals(true,
            cheYp.getMolecularType().equals(Molecule.CHEYP));
    }

    final public void testGetAndSetLevel() {
        Copynumber cheYp = new Copynumber(Molecule.CHEYP);
        TestCase.assertEquals(0, cheYp.getLevel());
        cheYp.setLevel(34);
        TestCase.assertEquals(34, cheYp.getLevel());
    }

    final public void testSetMolecularType() {
        Copynumber cheYp = new Copynumber(Molecule.CHEYP, 17);
        TestCase.assertEquals(true,
            cheYp.getMolecularType().equals(Molecule.CHEYP));
        cheYp.setMolecularType(Molecule.ASPARTATE);
        TestCase.assertEquals(true,
            cheYp.getMolecularType().equals(Molecule.ASPARTATE));
    }

    final public void testGetConcentrationDouble() {
        double volume = 1.41E-15; //Liters
        Copynumber n_cheYp = new Copynumber(Molecule.CHEYP, 200);
        Concentration c_cheYp = n_cheYp.getConcentration(volume);
        System.out.println(
            "[CheYp] should be 0.235 microMol = 0.235 10^-6 M. We find: " +
            c_cheYp.getLevel());
    }
}
