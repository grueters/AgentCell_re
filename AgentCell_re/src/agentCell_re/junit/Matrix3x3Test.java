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
 * Created on Feb 26, 2004
 *
 * 
 * 
 */
package agentCell_re.junit;

import agentCell_re.math.Calculus;
import agentCell_re.math.Matrix;
import agentCell_re.math.Matrix3x3;
import agentCell_re.math.Vect;
import agentCell_re.math.Vect3;
import junit.framework.TestCase;


/**
 * @author emonet
 *
 * 
 * 
 */
public class Matrix3x3Test extends TestCase {
    /**
     * Constructor for Matrix3x3Test.
     * @param arg0
     */
    public Matrix3x3Test(String arg0) {
        super(arg0);
    }

    public static void main(String args[]) {
        junit.swingui.TestRunner.run(Matrix3x3Test.class);
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
     * Test for void Matrix3x3()
     */
    final public void testMatrix3x3() {
        Matrix m = new Matrix3x3();
        TestCase.assertEquals(true, m.isZero());
    }

    /*
     * Test for void Matrix3x3(double, double, double, double, double, double, double, double, double)
     */
    final public void testMatrix3x3doubledoubledoubledoubledoubledoubledoubledoubledouble() {
        Matrix m = new Matrix3x3(1, 2, 3, 4, 5, 6, 7, 8, 9);
        TestCase.assertEquals(1, m.getElement(0, 0), 0);
        TestCase.assertEquals(2, m.getElement(0, 1), 0);
        TestCase.assertEquals(3, m.getElement(0, 2), 0);
        TestCase.assertEquals(4, m.getElement(1, 0), 0);
        TestCase.assertEquals(5, m.getElement(1, 1), 0);
        TestCase.assertEquals(6, m.getElement(1, 2), 0);
        TestCase.assertEquals(7, m.getElement(2, 0), 0);
        TestCase.assertEquals(8, m.getElement(2, 1), 0);
        TestCase.assertEquals(9, m.getElement(2, 2), 0);
    }

    /*
     * Test for void Matrix3x3(double[][])
     */
    final public void testMatrix3x3doubleArrayArray() {
        double dat[][] = {
                { 1, 2, 3 },
                { 4, 5, 6 },
                { 7, 8, 9 }
            };
        Matrix m = new Matrix3x3(dat);
        TestCase.assertEquals(1, m.getElement(0, 0), 0);
        TestCase.assertEquals(2, m.getElement(0, 1), 0);
        TestCase.assertEquals(3, m.getElement(0, 2), 0);
        TestCase.assertEquals(4, m.getElement(1, 0), 0);
        TestCase.assertEquals(5, m.getElement(1, 1), 0);
        TestCase.assertEquals(6, m.getElement(1, 2), 0);
        TestCase.assertEquals(7, m.getElement(2, 0), 0);
        TestCase.assertEquals(8, m.getElement(2, 1), 0);
        TestCase.assertEquals(9, m.getElement(2, 2), 0);
    }

    final public void testDiv() {
        Matrix m = new Matrix3x3(1, 2, 3, 4, 5, 6, 7, 8, 9);
        Matrix n = new Matrix3x3(0.5, 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5);
        TestCase.assertEquals(true, n.equals(m.div(2)));
    }

    final public void testGramSchmidt() {
        //double err = Calculus.TOLERANCE;
        Matrix m = new Matrix3x3(3, 4, 2, 2, 5, 2, 1, 2, 6);
        Matrix s = new Matrix3x3(3 / Math.sqrt(14.), -8 / (3 * Math.sqrt(21.)),
                -1 / (3 * Math.sqrt(6.)), 2 / Math.sqrt(14.),
                11 / (3 * Math.sqrt(21.)), -2 / (3 * Math.sqrt(6.)),
                1 / Math.sqrt(14.), 2 / (3 * Math.sqrt(21.)),
                7 / (3 * Math.sqrt(6.)));
        m.gramSchmidt();
        TestCase.assertEquals(true, s.equals(m.gramSchmidt()));
    }

    final public void testMinus() {
        Matrix m = new Matrix3x3(1, 2, 3, 4, 5, 6, 7, 8, 9);
        Matrix n = new Matrix3x3(0.5, 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5);
        Matrix s = new Matrix3x3(0.5, 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5);
        TestCase.assertEquals(true, (m.minus(n)).equals(s));
    }

    /*
     * Test for Matrix mult(double)
     */
    final public void testMultdouble() {
        Matrix m = new Matrix3x3(1, 2, 3, 4, 5, 6, 7, 8, 9);
        Matrix n = new Matrix3x3(0.5, 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5);
        TestCase.assertEquals(true, (n.mult(2)).equals(m));
    }

    /*
     * Test for Matrix mult(Matrix)
     */
    final public void testMultMatrix() {
        Matrix m = new Matrix3x3(1, 2, 3, 4, 5, 6, 7, 8, 9);
        Matrix n = new Matrix3x3(2, 1, 3, 4, 3, 2, 1, 2, 3);
        Matrix mn = new Matrix3x3(13, 13, 16, 34, 31, 40, 55, 49, 64);
        TestCase.assertEquals(true, mn.equals(m.mult(n)));
    }

    final public void testMultOtherThisMatrix() {
        Matrix m = new Matrix3x3(1, 2, 3, 4, 5, 6, 7, 8, 9);
        Matrix n = new Matrix3x3(2, 1, 3, 4, 3, 2, 1, 2, 3);
        Matrix mn = new Matrix3x3(13, 13, 16, 34, 31, 40, 55, 49, 64);
        TestCase.assertEquals(true, mn.equals(n.multOtherThis(m)));
    }

    /*
     * Test for Vect mult(Vect)
     */
    final public void testMultVect() {
        Matrix m = new Matrix3x3(1, 2, 3, 4, 5, 6, 7, 8, 9);
        Vect a = new Vect3(1, 2, 3);
        Vect sol = new Vect3(14, 32, 50);
        TestCase.assertEquals(true, sol.equals(m.mult(a)));
    }

    final public void testPlus() {
        Matrix m = new Matrix3x3(1, 2, 3, 4, 5, 6, 7, 8, 9);
        Matrix n = new Matrix3x3(0.5, 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5);
        Matrix s = new Matrix3x3(1.5, 3, 4.5, 6, 7.5, 9, 10.5, 12, 13.5);
        TestCase.assertEquals(true, (m.plus(n)).equals(s));
    }

    final public void testTranspose() {
        Matrix m = new Matrix3x3(1, 2, 3, 4, 5, 6, 7, 8, 9);
        Matrix s = new Matrix3x3(1, 4, 7, 2, 5, 8, 3, 6, 9);
        TestCase.assertEquals(true, m.transpose().equals(s));
    }

    /*
     * Test for Matrix copy()
     */
    final public void testCopy() {
        Matrix m = new Matrix3x3(1, 2, 3, 4, 5, 6, 7, 8, 9);
        Matrix n = m.copy();
        TestCase.assertEquals(true, m.equals(n));
        TestCase.assertEquals(true, m != n);
    }

    /*
     * Test for Matrix copy(Matrix)
     */
    final public void testCopyMatrix() {
        Matrix m = new Matrix3x3(1, 2, 3, 4, 5, 6, 7, 8, 9);
        Matrix n = new Matrix3x3(1, 4, 7, 2, 5, 8, 3, 6, 9);
        TestCase.assertEquals(true, (m.copy(n)).equals(n));
        TestCase.assertEquals(true, m != n);
    }

    final public void testGetColumnSize() {
        Matrix m = new Matrix3x3(1, 2, 3, 4, 5, 6, 7, 8, 9);
        TestCase.assertEquals(3, m.getColumnSize());
    }

    final public void testGetElement() {
        Matrix m = new Matrix3x3(1, 2, 3, 4, 5, 6, 7, 8, 9);
        int count = 1;

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                TestCase.assertEquals(count, m.getElement(i, j), 0);
                count++;
            }
        }
    }

    final public void testGetRowSize() {
        Matrix m = new Matrix3x3(1, 2, 3, 4, 5, 6, 7, 8, 9);
        TestCase.assertEquals(3, m.getRowSize());
    }

    final public void testSetElement() {
        Matrix n = new Matrix3x3(1, 2, 3, 4, 5, 6, 7, 8, 9);
        Matrix m = new Matrix3x3();
        int count = 1;

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                m.setElement(i, j, count);
                count++;
            }
        }

        TestCase.assertEquals(true, n.equals(m));
    }

    final public void testDet() {
        Matrix n = new Matrix3x3(1, 2, 3, 4, 5, 6, 7, 8, 9);
        double det = 0;
        TestCase.assertEquals(n.det(), det, Calculus.TOLERANCE);
    }

    final public void testInvert() {
        Matrix n = new Matrix3x3(1, 2, 3, 4, 5, 6, 7, 8, 10);
        Matrix nInv = new Matrix3x3(-2 / 3.0, -4 / 3.0, 1, -2 / 3.0, 11 / 3.0,
                -2, 1, -2, 1);
        TestCase.assertEquals(true, nInv.equals(n.invert()));
    }

    /*
     * Test for boolean equals(Object)
     */
    final public void testEqualsObject() {
        Matrix a = new Matrix3x3(1, 2, 3, 4, 5, 6, 7, 8, 9);
        Matrix b = new Matrix3x3(1.0, 2, 3, 4, 5, 6, 7, 8,
                9 + Calculus.TOLERANCE);
        TestCase.assertEquals(true, a.equals(b));
    }

    final public void testIsIdentity() {
        double err = Calculus.TOLERANCE * 0.999999999;
        Matrix m = new Matrix3x3(err + 1, 0, err, 0, 1, 0, 0, err, 1);
        TestCase.assertEquals(true, m.isIdentity());
    }

    final public void testIsZero() {
        double err = Calculus.TOLERANCE;
        Matrix m = new Matrix3x3(err, 0, err, 0, err, 0, 0, err, 0);
        TestCase.assertEquals(true, m.isZero());
    }

    final public void testCopyColumnInt() {
        Matrix m = new Matrix3x3(1, 2, 3, 4, 5, 6, 7, 8, 9);
        Vect m0 = m.copyColumn(0);
        Vect m1 = m.copyColumn(1);
        Vect m2 = m.copyColumn(2).mult(2);

        for (int i = 0; i < 3; i++) {
            TestCase.assertEquals(m0.getElement(i), m.getElement(i, 0), 0);
            TestCase.assertEquals(m1.getElement(i), m.getElement(i, 1), 0);
            TestCase.assertEquals(m2.getElement(i) / 2, m.getElement(i, 2),
                Calculus.TOLERANCE);
        }
    }

    final public void testCopyRowInt() {
        Matrix m = new Matrix3x3(1, 2, 3, 4, 5, 6, 7, 8, 9);
        Vect m0 = m.copyRow(0);
        Vect m1 = m.copyRow(1);
        Vect m2 = m.copyRow(2).mult(2);

        for (int i = 0; i < 3; i++) {
            TestCase.assertEquals(m0.getElement(i), m.getElement(0, i), 0);
            TestCase.assertEquals(m1.getElement(i), m.getElement(1, i), 0);
            TestCase.assertEquals(m2.getElement(i) / 2, m.getElement(2, i),
                Calculus.TOLERANCE);
        }
    }

    final public void testCopyColumnIntVect() {
        Matrix m = new Matrix3x3(1, 2, 3, 4, 5, 6, 7, 8, 9);
        Matrix n = new Matrix3x3();
        n.copyColumn(0, new Vect3(1, 4, 7));
        n.copyColumn(1, new Vect3(2, 5, 8));
        n.copyColumn(2, new Vect3(3, 6, 9));
        TestCase.assertEquals(true, m.equals(n));
    }

    final public void testCopyRowIntVect() {
        Matrix m = new Matrix3x3(1, 2, 3, 4, 5, 6, 7, 8, 9);
        Matrix n = new Matrix3x3();
        n.copyRow(0, new Vect3(1, 2, 3));
        n.copyRow(1, new Vect3(4, 5, 6));
        n.copyRow(2, new Vect3(7, 8, 9));
        TestCase.assertEquals(true, m.equals(n));
    }

    final public void testViewColumnInt() {
        Matrix m = new Matrix3x3(1, 2, 3, 4, 5, 6, 7, 8, 9);

        //check copies
        Vect m0 = m.viewColumn(0);
        Vect m1 = m.viewColumn(1);
        Vect m2 = m.viewColumn(2);

        for (int i = 0; i < 3; i++) {
            TestCase.assertEquals(m0.getElement(i), m.getElement(i, 0), 0);
            TestCase.assertEquals(m1.getElement(i), m.getElement(i, 1), 0);
            TestCase.assertEquals(m2.getElement(i), m.getElement(i, 2), 0);
        }

        //check if changing a view changes the corresponding part of the matrix
        m0.mult(0);
        m1.mult(1);
        m2.mult(2);

        for (int i = 0; i < 3; i++) {
            TestCase.assertEquals(m0.getElement(i), m.getElement(i, 0), 0);
            TestCase.assertEquals(m1.getElement(i), m.getElement(i, 1), 0);
            TestCase.assertEquals(m2.getElement(i), m.getElement(i, 2), 0);
        }
    }

    final public void testViewRowInt() {
        Matrix m = new Matrix3x3(1, 2, 3, 4, 5, 6, 7, 8, 9);

        //check copies
        Vect m0 = m.viewRow(0);
        Vect m1 = m.viewRow(1);
        Vect m2 = m.viewRow(2);

        for (int i = 0; i < 3; i++) {
            TestCase.assertEquals(m0.getElement(i), m.getElement(0, i), 0);
            TestCase.assertEquals(m1.getElement(i), m.getElement(1, i), 0);
            TestCase.assertEquals(m2.getElement(i), m.getElement(2, i), 0);
        }

        //check if changing a view changes the corresponding part of the matrix
        m0.mult(0);
        m1.mult(1);
        m2.mult(2);

        for (int i = 0; i < 3; i++) {
            TestCase.assertEquals(m0.getElement(i), m.getElement(0, i), 0);
            TestCase.assertEquals(m1.getElement(i), m.getElement(1, i), 0);
            TestCase.assertEquals(m2.getElement(i), m.getElement(2, i), 0);
        }
    }

    final public void testIsOrthogonal() {
        //deviation is TOLERANCE / 10 so it should be considered as orthogonal
        Matrix m = new Matrix3x3(1 - (Calculus.TOLERANCE / 10), 0, 0, 0, 1, 0,
                0, 0, 1);
        TestCase.assertEquals(true, m.isOrthogonal());

        //deviation just bigger than TOLERANCE
        Matrix n = new Matrix3x3(1 - Calculus.TOLERANCE, 0, 0, 0, 1, 0, 0, 0, 1);
        TestCase.assertEquals(false, n.isOrthogonal());

        // orthogonal columns but not orthonormal
        Matrix o = new Matrix3x3(1, 0, 2, 2, 0, -1, 0, 1, 0);
        TestCase.assertEquals(false, o.isOrthogonal());
    }
}
