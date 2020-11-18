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
 * Created on Jun 10, 2004
 *
 */
package agentCell_re.util.hdf;


/**
 * @author North
 *
 */
public class IntRecorder {
    // The data.
    private int data[] = null;

    // The recorded length.
    private int recordedLength = 0;

    // The running index length.
    private int nextIndex = 0;

    // The data recorder constructor.
    public IntRecorder(int initialSize) {
        this.data = new int[initialSize];
    }

    // Record an item.
    public void record(int value) {
        this.record(nextIndex, value);
        nextIndex++;
    }

    // Record an item.
    public void record(int index, int value) {
        // Check the index.
        if (index < 0) {
            // Note an error.
            System.out.println(
                "Index less than zero for data recorder.  The data was ignored.");
        } else {
            // Check the data set size.
            if (index >= this.data.length) {
                // Increase the storage size.
                int tempData[] = new int[Math.max(index + 1,
                        (int) (1.25 * this.data.length))];

                for (int i = 0; i < this.data.length; i++) {
                    tempData[i] = data[i];
                }

                this.data = tempData;
            }

            // Store the value.
            this.data[index] = value;

            // Note the recording.
            this.recordedLength = Math.max(this.recordedLength, index + 1);
        }
    }

    // The reading method.
    public int read(int index) {
        return this.data[index];
    }

    // The data store returning method.
    public int[] getData() {
        // Delcare the local variables.
        int tempData[];

        // Check the data set size.
        if (this.recordedLength < this.data.length) {
            // Clip the storage size.
            tempData = new int[this.recordedLength];

            for (int i = 0; i < recordedLength; i++) {
                tempData[i] = data[i];
            }
        } else {
            tempData = this.data;
        }

        // Return the results.
        return tempData;
    }
}
