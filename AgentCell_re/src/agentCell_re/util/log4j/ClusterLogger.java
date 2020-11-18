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
 * ClusterLogger.java
 * @author MichaelNorth
 * Created on Jan 1, 2004 at 6:59:37 PM
 *
 */
package agentCell_re.util.log4j;

import java.io.File;

import org.apache.log4j.Appender;
import org.apache.log4j.FileAppender;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.apache.log4j.PatternLayout;
import org.apache.log4j.chainsaw.Main;
import org.apache.log4j.net.SocketAppender;

import agentCell_re.models.ChemotaxisModel;


/**
 * The ClusterLogger class.
 * Created on Jan 1, 2004 6:59:37 PM
 * @author MichaelNorth
 *
 */
public class ClusterLogger {
    /**
     * The simulation control bar.
     */
    private static boolean startChainsaw = false;

    /**
     * The console appender.
     */
    public static Appender consoleAppender = null;

    /**
     * The text appender.
     */
    public static Appender fileAppender = null;

    /**
     * The HTML appender.
     */
    public static Appender HTMLAppender = null;

    /**
     * The XML appender.
     */
    public static Appender XMLAppender = null;

    /**
     * The XML appender.
     */
    public static Appender socketAppender = null;

    /**
     *
     */
    public ClusterLogger() {
        super();
    }

    /**
     * The main logger getter.
     */
    public static Logger getMainLoggerCategory(String category) {
        // Create a new logger.
        Logger logger = Logger.getLogger(category);

        // Configure basic logging.
        logger.setAdditivity(false);
        logger.setLevel(Level.DEBUG);

        // Setup the default base file name.
        String baseFileName = new File(ChemotaxisModel.runDirAbsolutePath, category).getAbsolutePath();
        
        // Check for errors.
        try {
            // Add a file appender.
            if (ClusterLogger.fileAppender == null) {
                ClusterLogger.fileAppender = new FileAppender(new PatternLayout(),
                        baseFileName+".OUT");
            }

            logger.addAppender(ClusterLogger.fileAppender);
//
            //			// Add an HTML appender.
            //			if (ClusterLogger.HTMLAppender == null) {
            //				ClusterLogger.HTMLAppender = new WriterAppender(
            //					new HTMLLayout(), new FileOutputStream(baseFileName + ".html"));
            //			}
            //			logger.addAppender(ClusterLogger.HTMLAppender);
            //			// Add an XML appender.
            //			if (ClusterLogger.XMLAppender == null) {
            //				ClusterLogger.XMLAppender = new WriterAppender(
            //					new XMLLayout(), new FileOutputStream(
            //					baseFileName + ".xml"));
            //			}
            //			logger.addAppender(ClusterLogger.XMLAppender);
            // Select the appropriate look and feel.
//            try {
//                String nativeLF = UIManager.getSystemLookAndFeelClassName();
//                UIManager.setLookAndFeel(nativeLF);
//            } catch (Exception e) {
//            }

            // Start Chainsaw, if requested.
            if (ClusterLogger.isStartChainsaw()) {
                // Start Chainsaw and add an Chainsaw appender.
                if (ClusterLogger.socketAppender == null) {
                    // Start Chainsaw.
                    Main.main(null);

                    // Create a Chainsaw appender.
                    ClusterLogger.socketAppender = new SocketAppender("localhost",
                            4445);
                }

                logger.addAppender(ClusterLogger.socketAppender);
            }

            // Note errors.
        } catch (Exception e) {
            e.printStackTrace();
        }

        // Return the results.
        return logger;
    }

    /**
     * The simple logger creator.
     */
    public static Logger createSimpleLogger(String coreFileName) {
        // Create a new logger.
        Logger logger = Logger.getLogger("Simple");

        // Configure basic logging.
        logger.setLevel(Level.DEBUG);
        logger.setAdditivity(false);

        // Check for errors.
        try {
            // Add a file appender.
            Appender appender = new FileAppender(new PatternLayout("%m%n"),
                    ChemotaxisModel.runDirAbsolutePath + coreFileName +
                    ".csv");
            logger.addAppender(appender);

            // Note errors.
        } catch (Exception e) {
            e.printStackTrace();
        }

        // Return the results.
        return logger;
    }

    /**
     * @return
     */
    public static boolean isStartChainsaw() {
        return startChainsaw;
    }

    /**
     * @param b
     */
    public static void setStartChainsaw(boolean b) {
        startChainsaw = b;
    }
}
