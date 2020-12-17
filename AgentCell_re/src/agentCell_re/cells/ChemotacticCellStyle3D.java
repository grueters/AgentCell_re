package agentCell_re.cells;

import java.awt.Color;
import java.awt.Font;
import java.io.File;

import org.jogamp.java3d.loaders.Scene;
import org.jogamp.java3d.loaders.objectfile.ObjectFile;
import org.jogamp.java3d.BranchGroup;
import org.jogamp.java3d.Geometry;
import org.jogamp.java3d.Node;
import org.jogamp.java3d.Shape3D;
import org.jogamp.java3d.Transform3D;
import org.jogamp.java3d.TransformGroup;

import repast.simphony.visualization.visualization3D.AppearanceFactory;
import repast.simphony.visualization.visualization3D.ShapeFactory;
import repast.simphony.visualization.visualization3D.style.Style3D;
import repast.simphony.visualization.visualization3D.style.TaggedAppearance;
import repast.simphony.visualization.visualization3D.style.TaggedBranchGroup; 

/**
 * The 3D style class for sugar agents.   
 * The style is a red sphere. 
 *
 * @author Eric Tatara
 * @author Nick Collier
 * @version 
 */

public class ChemotacticCellStyle3D implements Style3D<ChemotacticCell> {

	private static Color cellColor = new Color(0, 255, 0);

	public TaggedBranchGroup getBranchGroup(ChemotacticCell agent, TaggedBranchGroup taggedGroup) {

		if (taggedGroup == null || taggedGroup.getTag() == null) {
			taggedGroup = getGroup(agent);
			return taggedGroup;
		} 
		return null;
	}

	/**
	 * @param agent
	 * @return
	 */
	private TaggedBranchGroup getGroup(ChemotacticCell agent) {
		TaggedBranchGroup taggedGroup;
		taggedGroup = new TaggedBranchGroup("CELL");
		double creaseAngle = 60.0; 
		int flags = ObjectFile.RESIZE;
        ObjectFile objFile = new ObjectFile(flags, (float)(creaseAngle*Math.PI)/180);
        Scene scene = null;
        String path = "/home/grueters/git/AgentCell_re_repo/AgentCell_re/Intergalactic_Spaceship-(Wavefront).obj";
        try {
        	scene = objFile.load(path);
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("OBJ load Err：" + e.getMessage());
        }
        BranchGroup branchGroup = scene.getSceneGroup( );
        
        
        
        Shape3D oldShape = (Shape3D) branchGroup.getChild( 0 );
       	Shape3D shape = new Shape3D();
       	shape.setGeometry(oldShape.getGeometry()); 
       	
		
		  // Shape3D shape = ShapeFactory.createBox(0.02f, 0.01f, 0.06f, null, 0,null);
		  //Shape3D shape = ShapeFactory.createArrowHead(0.1f, null);
		  shape.setCapability(Shape3D.ALLOW_APPEARANCE_WRITE);
		  Transform3D transform3d = new Transform3D();
		  transform3d.rotX(Math.toRadians(90)); 
		  TransformGroup transShape = new TransformGroup(transform3d); 
		  transShape.addChild(shape);
		  
		  taggedGroup.getBranchGroup().addChild(shape);
		 
		return taggedGroup;
	}

	public float[] getRotation(ChemotacticCell agent) {
		float[] axisAngle = agent.getOrientation().getAxisAngle4f();
		System.out.println("orientation = " + agent.getOrientation());
		System.out.println("Axisangle4f = " + axisAngle[0] + ", " + axisAngle[1]
				+  ", " + axisAngle[2] + ", " + axisAngle[3]);
		return axisAngle;
	}

	public String getLabel(ChemotacticCell agent, String currentLabel) {
		return null; 
	}

	public Color getLabelColor(ChemotacticCell agent, Color currentColor) {
		return Color.YELLOW;
	}

	public Font getLabelFont(ChemotacticCell agent, Font currentFont) {
		return null;
	}

	public LabelPosition getLabelPosition(ChemotacticCell agent, LabelPosition curentPosition) {
		return LabelPosition.NORTH;
	}

	public float getLabelOffset(ChemotacticCell agent) {
		return .035f;
	}

	public TaggedAppearance getAppearance(ChemotacticCell agent, TaggedAppearance taggedAppearance, Object shapeID) {
		if (taggedAppearance == null) {
			taggedAppearance = new TaggedAppearance();
		} 

		if (agent instanceof ChemotacticCell)
		  AppearanceFactory.setMaterialAppearance(taggedAppearance.getAppearance(), cellColor);
	
		return taggedAppearance;
	}

	public float[] getScale(ChemotacticCell agent) {
		float v = (float) agent.getVolume();
		v *= 1.0E15;
		float[] scale = {v,v,v};
		return scale;
	}
}
