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
 * Created on Feb 27, 2004
 *
 * 
 * 
 */
package agentCell_re.junit;

import agentCell_re.math.Calculus;
import agentCell_re.math.Orientation;
import agentCell_re.math.Vect;
import agentCell_re.math.Vect3;
import junit.framework.TestCase;
import repast.simphony.random.RandomHelper;

/**
 * @author emonet
 *
 * 
 * 
 */
public class OrientationTest extends TestCase {
    /**
     * Constructor for OrientationTest.
     * @param arg0
     */
    public OrientationTest(String arg0) {
        super(arg0);
    }

    public static void main(String args[]) {
        junit.swingui.TestRunner.run(OrientationTest.class);
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
     * Test for Orientation copy()
     */
    final public void testCopy() {
        Orientation o1 = new Orientation();
        o1.rotateAroundGlobalAxis(new Vect3(1, 1, 1), (2 * Math.PI) / 3);

        Orientation o2 = (Orientation) o1.copy();

        //test that copy is same as original
        TestCase.assertEquals(true, o2.equals(o1));
        o2.rotateAroundGlobalAxis(1, Math.PI);

        //copy is independent from original
        TestCase.assertEquals(false, o2.equals(o1));

        //test the super class copy(Matrix) method
        o2.copy(o1);
        TestCase.assertEquals(true, o2.equals(o1));
    }

    /*
     * Test for Orientation rotateAroundGlobalAxis(int, double)
     */
    final public void testRotateAroundGlobalAxisintdouble() {
        Orientation o1 = new Orientation();
        o1.rotateAroundGlobalAxis(0, Math.PI / 2);
        TestCase.assertEquals(1, o1.getElement(0, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(1, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(2, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(0, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(1, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(1, o1.getElement(2, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(0, 2), Calculus.TOLERANCE);
        TestCase.assertEquals(-1, o1.getElement(1, 2), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(2, 2), Calculus.TOLERANCE);
        o1.rotateAroundGlobalAxis(1, Math.PI / 2);
        TestCase.assertEquals(0, o1.getElement(0, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(1, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(-1, o1.getElement(2, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(1, o1.getElement(0, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(1, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(2, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(0, 2), Calculus.TOLERANCE);
        TestCase.assertEquals(-1, o1.getElement(1, 2), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(2, 2), Calculus.TOLERANCE);
        o1.rotateAroundGlobalAxis(2, Math.PI / 2);
        TestCase.assertEquals(0, o1.getElement(0, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(1, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(-1, o1.getElement(2, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(0, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(1, o1.getElement(1, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(2, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(1, o1.getElement(0, 2), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(1, 2), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(2, 2), Calculus.TOLERANCE);
    }

    /*
     * Test for Orientation rotateAroundLocalAxis(int, double)
     */
    final public void testRotateAroundLocalAxisintdouble() {
        Orientation o1 = new Orientation();
        o1.rotateAroundLocalAxis(0, Math.PI / 2);
        TestCase.assertEquals(1, o1.getElement(0, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(1, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(2, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(0, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(1, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(1, o1.getElement(2, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(0, 2), Calculus.TOLERANCE);
        TestCase.assertEquals(-1, o1.getElement(1, 2), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(2, 2), Calculus.TOLERANCE);
        o1.rotateAroundLocalAxis(1, Math.PI / 2);
        TestCase.assertEquals(0, o1.getElement(0, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(1, o1.getElement(1, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(2, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(0, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(1, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(1, o1.getElement(2, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(1, o1.getElement(0, 2), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(1, 2), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(2, 2), Calculus.TOLERANCE);
        o1.rotateAroundLocalAxis(2, Math.PI / 2);
        TestCase.assertEquals(0, o1.getElement(0, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(1, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(1, o1.getElement(2, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(0, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(-1, o1.getElement(1, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(2, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(1, o1.getElement(0, 2), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(1, 2), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(2, 2), Calculus.TOLERANCE);
    }

    /*
     * Test for Orientation rotateAroundGlobalAxis(Vect, double)
     */
    final public void testRotateAroundGlobalAxisVectdouble() {
        Vect v = new Vect3(1, 1, 1);
        double angle = (2 * Math.PI) / 3;
        Orientation o1 = new Orientation();
        o1.rotateAroundGlobalAxis(v, angle);
        TestCase.assertEquals(0, o1.getElement(0, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(1, o1.getElement(1, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(2, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(0, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(1, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(1, o1.getElement(2, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(1, o1.getElement(0, 2), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(1, 2), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(2, 2), Calculus.TOLERANCE);
        o1.rotateAroundGlobalAxis(v, angle);
        TestCase.assertEquals(0, o1.getElement(0, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(1, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(1, o1.getElement(2, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(1, o1.getElement(0, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(1, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(2, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(0, 2), Calculus.TOLERANCE);
        TestCase.assertEquals(1, o1.getElement(1, 2), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(2, 2), Calculus.TOLERANCE);
        o1.rotateAroundGlobalAxis(v, angle);
        TestCase.assertEquals(1, o1.getElement(0, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(1, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(2, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(0, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(1, o1.getElement(1, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(2, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(0, 2), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(1, 2), Calculus.TOLERANCE);
        TestCase.assertEquals(1, o1.getElement(2, 2), Calculus.TOLERANCE);
    }

    /*
     * Test for Orientation rotateAroundLocalAxis(Vect, double)
     */
    final public void testRotateAroundLocalAxisVectdouble() {
        Vect v = new Vect3(1, 1, 1);
        double angle = (2 * Math.PI) / 3;
        Orientation o1 = new Orientation();
        o1.rotateAroundLocalAxis(2, Math.PI / 2);

        // start test
        o1.rotateAroundLocalAxis(v, angle);
        TestCase.assertEquals(-1, o1.getElement(0, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(1, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(2, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(0, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(1, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(1, o1.getElement(2, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(0, 2), Calculus.TOLERANCE);
        TestCase.assertEquals(1, o1.getElement(1, 2), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(2, 2), Calculus.TOLERANCE);
        o1.rotateAroundLocalAxis(v, angle);
        TestCase.assertEquals(0, o1.getElement(0, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(1, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(1, o1.getElement(2, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(0, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(1, o1.getElement(1, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(2, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(-1, o1.getElement(0, 2), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(1, 2), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(2, 2), Calculus.TOLERANCE);
        o1.rotateAroundLocalAxis(v, angle);
        TestCase.assertEquals(0, o1.getElement(0, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(1, o1.getElement(1, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(2, 0), Calculus.TOLERANCE);
        TestCase.assertEquals(-1, o1.getElement(0, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(1, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(2, 1), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(0, 2), Calculus.TOLERANCE);
        TestCase.assertEquals(0, o1.getElement(1, 2), Calculus.TOLERANCE);
        TestCase.assertEquals(1, o1.getElement(2, 2), Calculus.TOLERANCE);
    }

    /*
     * Test for Orientation rotateAroundLocalAxes(int, double, double, double)
     */
    final public void testRotateAroundLocalAxesintdoubledouble() {
        Orientation o1 = new Orientation();

        o1.rotateAroundLocalAxes(0, Math.PI / 2, Math.PI / 2, Math.PI / 2);
        TestCase.assertEquals(true,
            o1.equals(new Orientation(0, 0, 1, 0, -1, 0, 1, 0, 0)));

        o1.setToIdentity();
        o1.rotateAroundLocalAxes(1, Math.PI / 2, Math.PI / 2, Math.PI / 2);
        TestCase.assertEquals(true,
            o1.equals(new Orientation(0, -1, 0, 1, 0, 0, 0, 0, 1)));
    }

    final public void testViewDirection() {
        Vect v = new Vect3(1, 1, 1);
        double angle = (2 * Math.PI) / 3;
        Orientation o1 = new Orientation();
        o1.rotateAroundLocalAxis(2, Math.PI / 2);

        // start test
        o1.rotateAroundLocalAxis(v, angle);
        TestCase.assertEquals(true,
            o1.viewDirection().equals(new Vect3(0, 1, 0)));
    }

    final public void testApplyGramSchmidtIfNecessary() {
        Orientation o = new Orientation();

        // Create a uniform distribution with a minimum of 0 and a maximum of 1.
       //  Random.createUniform(0, 1);

        // test the rotation counter
        int count = 0;

        for (int i = 0; i < 10; i++) {
            o.rotateAroundLocalAxis(2,
                RandomHelper.nextDoubleFromTo(0, 2 * Math.PI));
            count++;
            TestCase.assertEquals(count, o.getRotationsCount());
        }

        // change the rotationThreshold
        o.setRotationsCountThreshold(3);
        o.rotateAroundLocalAxis(0,
            RandomHelper.nextDoubleFromTo(0, 2 * Math.PI));
        TestCase.assertEquals(0, o.getRotationsCount());
        o.rotateAroundLocalAxis(0,
            RandomHelper.nextDoubleFromTo(0, 2 * Math.PI));
        TestCase.assertEquals(1, o.getRotationsCount());
        o.rotateAroundLocalAxis(0,
            RandomHelper.nextDoubleFromTo(0, 2 * Math.PI));
        TestCase.assertEquals(2, o.getRotationsCount());
        o.rotateAroundLocalAxis(0,
            RandomHelper.nextDoubleFromTo(0, 2 * Math.PI));
        TestCase.assertEquals(0, o.getRotationsCount());

        // test the orthogonality
        o = new Orientation();

        for (int i = 0; i < (o.getRotationsCountThreshold() - 2); i++) {
            //random rotation around direction of motion
            o.rotateAroundLocalAxis(2,
                RandomHelper.nextDoubleFromTo(0, 2 * Math.PI));

            //generate an angle from the distribution: p(alpha) = cos(alpha/2)/2,  0 &le; alpha &le; Pi		
            double alpha = 2 * Math.asin(RandomHelper.nextDoubleFromTo(0, 1));

            //rotate by alpha around local y-axis
            o.rotateAroundLocalAxis(0, alpha);
        }

        System.out.println(
            "Test is successful ONLY if you DO NOT SEE this message:");
        System.out.println(
            "     WARNING: Orientation not orthogonal after ... rotations. " +
            "Consider decreasing the rotationsCountThreshold. (See tests in OrientationTest.java)");
    }
}
