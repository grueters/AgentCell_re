package agentCell_re.cells;

import java.awt.Color;
import java.awt.Font;

import javax.media.j3d.BranchGroup;
import javax.media.j3d.Shape3D;
import javax.media.j3d.Transform3D;
import javax.media.j3d.TransformGroup;
import javax.vecmath.Vector3d;

import com.sun.j3d.loaders.Scene;
import com.sun.j3d.loaders.objectfile.ObjectFile;

import repast.simphony.visualization.visualization3D.AppearanceFactory;
import repast.simphony.visualization.visualization3D.VisualItem3D;
import repast.simphony.visualization.visualization3D.style.Style3D;
import repast.simphony.visualization.visualization3D.style.TaggedAppearance;
import repast.simphony.visualization.visualization3D.style.TaggedBranchGroup;

/**
 * The 3D style class for sugar agents. The style is a red sphere.
 *
 * @author Eric Tatara
 * @author Nick Collier
 * @version
 */

public class ChemotacticCellStyle3D implements Style3D<ChemotacticCell> {

	// private static Color cellColor = new Color(0, 255, 0);
	

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

		ObjectFile objFile = new ObjectFile(flags, (float) (creaseAngle * Math.PI) / 180);
		Scene scene = null;
		
		String path = null;
		int modelXRotation = 0;
		int bundledModelXRotation = 90;
		int apartModelXRotation = 180;
		if (agent instanceof ChemotacticCell) {
			if(agent.getFlagellaState().equals("Bundled")) {
				path = "/home/leonm/git/AgentCell_re/AgentCell_re/e.-Coli/geschlossene Tentakel/eColi.obj";
				modelXRotation = bundledModelXRotation/* = 90*/;
			} else if (agent.getFlagellaState().equals("Apart")) {
				path = "/home/leonm/git/AgentCell_re/AgentCell_re/e.-Coli/gespreitzte Tentakel/eColi_gespreitzt.obj";
				modelXRotation = apartModelXRotation/* = 180*/;
			} else {
				path = "/home/leonm/git/AgentCell_re/AgentCell_re/ufo plane free.obj";
				modelXRotation = 0;
			}
		}
		
		
		try {
			scene = objFile.load(path);
		} catch (Exception e) {
			e.printStackTrace();
			System.out.println("OBJ load Err：" + e.getMessage());
		}
		BranchGroup sceneGroup = scene.getSceneGroup();
		taggedGroup.setBranchGroup(sceneGroup);
		Shape3D shape = (Shape3D) sceneGroup.getChild(0);
		shape.setCapability(Shape3D.ALLOW_APPEARANCE_WRITE);
		taggedGroup.getBranchGroup().removeAllChildren();
		// Shape3D shape = ShapeFactory.createBox(0.02f, 0.01f, 0.06f, null, 0,null);
		// Shape3D shape = ShapeFactory.createArrowHead(0.1f, null);
		// shape.setCapability(Shape3D.ALLOW_APPEARANCE_WRITE);

		double scalingfactor = 0.0090d;
		
		Transform3D transform3d = new Transform3D();
		transform3d.rotY(Math.toRadians(270));
		Transform3D temp = new Transform3D();
		//Unfortunately the models for the apart and the bundled
		//flagella are different in their x-rotation
		//Therefore use the int apartModelXRotation for the eColi_gespreitzt.obj
		// and bundledModelXRotation for eColi.obj here:
		temp.rotX(Math.toRadians(modelXRotation));
		transform3d.mul(temp);
		Vector3d scale = new Vector3d(scalingfactor, scalingfactor, scalingfactor);
        transform3d.setScale(scale);
		TransformGroup transShape = new TransformGroup(transform3d);
		transShape.addChild(shape);

		taggedGroup.getBranchGroup().addChild(transShape);

		return taggedGroup;
	}

	public float[] getRotation(ChemotacticCell agent) {
		float[] axisAngle = agent.getOrientation().getAxisAngle4f();
		System.out.println("orientation = " + agent.getOrientation());
		System.out.println(
				"Axisangle4f = " + axisAngle[0] + ", " + axisAngle[1] + ", " + axisAngle[2] + ", " + axisAngle[3]);
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
		
		if (agent instanceof ChemotacticCell) {
			Color cellColor;
			if(agent.getFlagellaState().equals("Bundled")) {
				cellColor = Color.GREEN;
				getBranchGroup(agent, null);
				//Display3D.getVisualObject(agent).setTaggedBranchGroup(getGroup(agent));
			} else if (agent.getFlagellaState().equals("Apart")) {
				cellColor = Color.RED;
				getGroup(agent);
			} else {
				cellColor = Color.YELLOW;
				getGroup(agent);
			}
			AppearanceFactory.setMaterialAppearance(taggedAppearance.getAppearance(), cellColor);
		}
		return taggedAppearance;
	}

	public float[] getScale(ChemotacticCell agent) {
		float v = (float) agent.getVolume();
		v *= 1.0E15;
		float[] scale = { v, v, v };
		return scale;
	}
}
