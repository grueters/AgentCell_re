package agentCell_re.cells;

import java.awt.Color;
import java.awt.Font;
import java.io.File;

import org.jogamp.java3d.Appearance;
import org.jogamp.java3d.BranchGroup;
import org.jogamp.java3d.ColoringAttributes;
import org.jogamp.java3d.Shape3D;
import org.jogamp.java3d.Transform3D;
import org.jogamp.java3d.TransformGroup;
import org.jogamp.java3d.TransparencyAttributes;
import org.jogamp.java3d.loaders.Scene;
import org.jogamp.java3d.loaders.objectfile.ObjectFile;
import org.jogamp.vecmath.Color3f;
import org.jogamp.vecmath.Vector3d;

import agentCell_re.flagella.Flagella;
import repast.simphony.visualization.visualization3D.AppearanceFactory;
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
			taggedGroup = getGroups(agent);
			return taggedGroup;
		} else if (agent.hasFlagellaStateChanged()) {
			// TODO add routines for changing transparency of either bundled or apart shpae
			BranchGroup sceneGroupBundled = (BranchGroup) taggedGroup.getBranchGroup().getChild(0);
			BranchGroup sceneGroupApart = (BranchGroup) taggedGroup.getBranchGroup().getChild(1);
			TransformGroup transformGroupBundled = (TransformGroup) sceneGroupBundled.getChild(0);
			TransformGroup transformGroupApart = (TransformGroup) sceneGroupApart.getChild(0);
			Shape3D shapeBundled = (Shape3D) transformGroupBundled.getChild(0);
			Shape3D shapeApart = (Shape3D) transformGroupApart.getChild(0);
			if (agent.getFlagellaState().equals("Apart")) {
				changeAppearance(shapeBundled, Color.GREEN, 1.0f);
				changeAppearance(shapeApart, Color.RED, 0.0f);
			} else {
				changeAppearance(shapeBundled, Color.GREEN, 0.0f);
				changeAppearance(shapeApart, Color.RED, 1.0f);
			}
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

		String path = System.getProperty("user.dir") + File.separator + "eColi";
		
		int modelXRotation = 0;
		int bundledModelXRotation = 90;
		int apartModelXRotation = 180;
		if (agent instanceof ChemotacticCell) {
			if (agent.getFlagellaState().equals("Bundled")) {
				path = path + File.separator + "eColi_bundled.obj";
				modelXRotation = bundledModelXRotation/* = 90 */;
			} else if (agent.getFlagellaState().equals("Apart")) {
				path = path + File.separator + "eColi_bundled.obj";
				modelXRotation = apartModelXRotation/* = 180 */;
			} else {
				path = "/home/grueters/git/AgentCell_re_repo/AgentCell_re/ufo plane free.obj";
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
		// Unfortunately the models for the apart and the bundled
		// flagella are different in their x-rotation
		// Therefore use the int apartModelXRotation for the eColi_gespreitzt.obj
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
			if (agent.getFlagellaState().equals("Bundled")) {
				cellColor = Color.GREEN;
				getBranchGroup(agent, null);
				// Display3D.getVisualObject(agent).setTaggedBranchGroup(getGroup(agent));
			} else if (agent.getFlagellaState().equals("Apart")) {
				cellColor = Color.RED;
				getGroup(agent);
			} else {
				cellColor = Color.YELLOW;
				getGroup(agent);
			}
			AppearanceFactory.setMaterialAppearance(taggedAppearance.getAppearance(), cellColor);
		}
		return null; // taggedAppearance;
	}

	public float[] getScale(ChemotacticCell agent) {
		float v = (float) agent.getVolume();
		v *= 1.0E15;
		float[] scale = { v, v, v };
		return scale;
	}

	private TaggedBranchGroup getGroups(ChemotacticCell agent) {
		TaggedBranchGroup taggedGroup;
		taggedGroup = new TaggedBranchGroup("CELL");
		double creaseAngle = 60.0;
		int flags = ObjectFile.RESIZE;

		ObjectFile objFileBundled = new ObjectFile(flags, (float) (creaseAngle * Math.PI) / 180);
		ObjectFile objFileApart = new ObjectFile(flags, (float) (creaseAngle * Math.PI) / 180);
		Scene sceneBundled = null;
		Scene sceneApart = null;

		// path = "/home/grueters/git/AgentCell_re_repo/AgentCell_re/ufo plane
		// free.obj";
		// modelXRotation = 0;
		String path = System.getProperty("user.dir") + File.separator + "eColi";
		int bundledModelXRotation = 90;
		int apartModelXRotation = 180;
		int modelXRotation = 0;

		String pathBundled = path + File.separator + "eColi_bundled.obj";
		String pathApart = path + File.separator + "eColi_apart.obj";

		try {
			sceneBundled = objFileBundled.load(pathBundled);
			sceneApart = objFileApart.load(pathApart);
		} catch (Exception e) {
			e.printStackTrace();
			System.out.println("OBJ load Err：" + e.getMessage());
		}
		BranchGroup holder = new BranchGroup();
		BranchGroup sceneGroupBundled = sceneBundled.getSceneGroup();
		BranchGroup sceneGroupApart = sceneApart.getSceneGroup();
		holder.addChild(sceneGroupBundled);
		holder.addChild(sceneGroupApart);

		taggedGroup.setBranchGroup(holder);

		double scalingfactor = 0.009d; 

		Shape3D shapeBundled = (Shape3D) sceneGroupBundled.getChild(0);
		shapeBundled.setCapability(Shape3D.ALLOW_APPEARANCE_WRITE);
		sceneGroupBundled.removeAllChildren();
		changeAppearance(shapeBundled, Color.GREEN, 0.0f);
		TransformGroup transShape = matchStyleToOrientation(shapeBundled, bundledModelXRotation, scalingfactor);
		sceneGroupBundled.addChild(transShape);

		Shape3D shapeApart = (Shape3D) sceneGroupApart.getChild(0);
		shapeApart.setCapability(Shape3D.ALLOW_APPEARANCE_WRITE);
		sceneGroupApart.removeAllChildren();
		changeAppearance(shapeApart, Color.RED, 1.0f);
		transShape = matchStyleToOrientation(shapeApart, apartModelXRotation, scalingfactor);
		sceneGroupApart.addChild(transShape);

		return taggedGroup;
	}

	private TransformGroup matchStyleToOrientation(Shape3D shape, int modelXRotation, double scalingfactor) {
		Transform3D transform3d = new Transform3D();
		transform3d.rotY(Math.toRadians(270));
		Transform3D temp = new Transform3D();
		// Unfortunately the models for the apart and the bundled
		// flagella are different in their x-rotation
		// Therefore use the int apartModelXRotation for the eColi_gespreitzt.obj
		// and bundledModelXRotation for eColi.obj here:
		temp.rotX(Math.toRadians(modelXRotation));
		transform3d.mul(temp);
		Vector3d scale = new Vector3d(scalingfactor, scalingfactor, scalingfactor);
		transform3d.setScale(scale);
		TransformGroup transShape = new TransformGroup(transform3d);
		transShape.addChild(shape);
		return transShape;
	}

	private void changeAppearance(Shape3D shape, Color col, float transparencyFrac) {
		Appearance ap = new Appearance();
		AppearanceFactory.setMaterialAppearance(ap, col);
		TransparencyAttributes ta = new TransparencyAttributes();
		ta.setTransparencyMode(ta.NICEST);
		ta.setTransparency(transparencyFrac);
		ap.setTransparencyAttributes(ta);
		shape.setAppearance(ap);
	}
}
