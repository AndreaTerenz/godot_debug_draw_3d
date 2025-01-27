#pragma once

#include "colors.h"
#include "data_graphs.h"
#include "debug_geometry_container.h"
#include "enums.h"
#include "geometry_generators.h"
#include "grouped_text.h"

#include <CanvasItem.hpp>
#include <CanvasLayer.hpp>
#include <Font.hpp>
#include <Godot.hpp>
#include <Node.hpp>
#include <Reference.hpp>
#include <Viewport.hpp>

using namespace godot;

class DebugDraw3D : public Node {
	GODOT_CLASS(DebugDraw3D, Node)

	static DebugDraw3D *singleton;
	static int instance_counter;

public:
private:
#define CONST_GET(_enum, _const)       \
	int64_t get_##_enum##_##_const() { \
		return _enum::_const;          \
	}

	void fake_set_const(int64_t val){};
	CONST_GET(BlockPosition, LeftTop);
	CONST_GET(BlockPosition, RightTop);
	CONST_GET(BlockPosition, LeftBottom);
	CONST_GET(BlockPosition, RightBottom);

	CONST_GET(GraphTextFlags, None);
	CONST_GET(GraphTextFlags, Current);
	CONST_GET(GraphTextFlags, Avarage);
	CONST_GET(GraphTextFlags, Max);
	CONST_GET(GraphTextFlags, Min);
	CONST_GET(GraphTextFlags, All);

#undef CONST_GET

	std::vector<Viewport *> custom_editor_viewports;

	// Logs
	real_t log_flush_time = 0;

	// 2d
	CanvasLayer *_canvasLayer = nullptr;
	bool _canvasNeedUpdate = true;
	Ref<Font> _font;

	CanvasItem *default_canvas = nullptr;

	// Text
	std::unique_ptr<GroupedText> grouped_text;

	// Graphs
	std::unique_ptr<DataGraphManager> data_graphs;

	// Meshes
	std::unique_ptr<DebugGeometryContainer> dgc;

	std::recursive_mutex datalock;
	bool is_ready = false;

	DebugDraw3D *get_singleton_gdscript() {
		return singleton;
	};
	void on_canva_item_draw(CanvasItem *ci);

#pragma region Exposed Parameter Values

	// GENERAL

	/// Recall all calls from DebugDraw3D instance to its singleton if needed
	bool recall_to_singleton = true;
	/// Enable or disable all debug draw
	bool debug_enabled = true;
	/// Freezing 3d debugging instances
	bool freeze_3d_render = false;
	/// Debug for debug...
	bool visible_instance_bounds = false;
	/// Geometry culling based on camera frustum.
	/// It is not recommended to use with the current implementation.
	bool use_frustum_culling = false;
	/// Force use camera placed on edited scene.
	/// Usable for editor.
	bool force_use_camera_from_scene = false;
	/// Base offset for all graphs
	Vector2 graphs_base_offset = Vector2(8, 8);
	/// Layers on which the geometry will be displayed
	int64_t geometry_render_layers = 1;

	// TEXT

	/// Position of text block
	BlockPosition text_block_position = BlockPosition::LeftTop;
	/// Offset from the corner selected in 'text_block_position'
	Vector2 text_block_offset = Vector2(8, 8);
	/// Text padding for each line
	Vector2 text_padding = Vector2(2, 1);
	/// How long text remain shown after being invoked.
	real_t text_default_duration = 0.5f;
	/// Default color of the text
	Color text_foreground_color = Colors::white;
	/// Background color of the text
	Color text_background_color = Colors::gray_bg;
	/// Custom text Font
	Ref<Font> text_custom_font = nullptr;

	// GEOMETRY

	/// Default color of line with hit
	Color line_hit_color = Colors::red;
	/// Default color of line after hit
	Color line_after_hit_color = Colors::green;

	// Misc

	/// Custom 'Viewport' to use for frustum culling.
	Viewport *custom_viewport = nullptr;
	/// Custom 'CanvasItem' to draw on it. Set to 'null' to disable.
	CanvasItem *custom_canvas = nullptr;
#pragma endregion // Exposed Parameter Values

public:
	static void _register_methods();
	void _init();

	static DebugDraw3D *get_singleton() {
		return singleton;
	};
	void mark_canvas_needs_update();

	void set_custom_editor_viewport(std::vector<Viewport *> viewports);
	std::vector<Viewport *> get_custom_editor_viewport();

	void _enter_tree();
	void _exit_tree();
	void _ready();
	void _process(real_t delta);

#pragma region Exposed Parameters
	void set_recall_to_singleton(bool state);
	bool is_recall_to_singleton();

	void set_debug_enabled(bool state);
	bool is_debug_enabled();

	void set_freeze_3d_render(bool state);
	bool is_freeze_3d_render();

	void set_visible_instance_bounds(bool state);
	bool is_visible_instance_bounds();

	void set_use_frustum_culling(bool state);
	bool is_use_frustum_culling();

	void set_force_use_camera_from_scene(bool state);
	bool is_force_use_camera_from_scene();

	void set_graphs_base_offset(Vector2 offset);
	Vector2 get_graphs_base_offset();

	void set_geometry_render_layers(int64_t layers);
	int64_t get_geometry_render_layers();

	void set_text_block_position(int /*BlockPosition*/ position);
	int /*BlockPosition*/ get_text_block_position();

	void set_text_block_offset(Vector2 offset);
	Vector2 get_text_block_offset();

	void set_text_padding(Vector2 padding);
	Vector2 get_text_padding();

	void set_text_default_duration(real_t duration);
	real_t get_text_default_duration();

	void set_text_foreground_color(Color new_color);
	Color get_text_foreground_color();

	void set_text_background_color(Color new_color);
	Color get_text_background_color();

	void set_text_custom_font(Ref<Font> custom_font);
	Ref<Font> get_text_custom_font();

	void set_line_hit_color(Color new_color);
	Color get_line_hit_color();

	void set_line_after_hit_color(Color new_color);
	Color get_line_after_hit_color();

	void set_custom_viewport(Viewport *viewport);
	Viewport *get_custom_viewport();

	void set_custom_canvas(CanvasItem *canvas);
	CanvasItem *get_custom_canvas();
#pragma endregion // Exposed Parametes

#pragma region Exposed Draw Functions

	/// Returns a dictionary with rendering statistics.
	/// Some data can be delayed by 1 frame.
	Dictionary get_render_stats();

	/// Clear all 3D objects
	void clear_3d_objects();

	/// Clear all 2D objects
	void clear_2d_objects();

	/// Clear all debug objects
	void clear_all();

#pragma region 3D
#pragma region Spheres

	/// Draw sphere
	/// position: Position of the sphere center
	/// radius: Sphere radius
	/// color: Sphere color
	/// duration: Duration of existence in seconds
	void draw_sphere(Vector3 position, real_t radius = 0.5f, Color color = Colors::empty_color, real_t duration = 0);

	/// Draw sphere
	/// transform: Transform of the Sphere
	/// color: Sphere color
	/// duration: Duration of existence in seconds
	void draw_sphere_xf(Transform transform, Color color = Colors::empty_color, real_t duration = 0);

	/// Draw sphere with higher lines count
	/// position: Position of the sphere center
	/// radius: Sphere radius
	/// color: Sphere color
	/// duration: Duration of existence in seconds
	void draw_sphere_hd(Vector3 position, real_t radius = 0.5f, Color color = Colors::empty_color, real_t duration = 0);

	/// Draw sphere with higher lines count
	/// transform: Transform of the sphere
	/// color: Sphere color
	/// duration: Duration of existence in seconds
	void draw_sphere_hd_xf(Transform transform, Color color = Colors::empty_color, real_t duration = 0);

#pragma endregion // Spheres
#pragma region Cylinders

	/// Draw vertical cylinder
	/// transform: Transform of the Cylinder
	/// color: Cylinder color
	/// duration: Duration of existence in seconds
	void draw_cylinder(Transform transform, Color color = Colors::empty_color, real_t duration = 0);

#pragma endregion // Cylinders
#pragma region Boxes

	/// Draw box
	/// position: Position of the Box
	/// size: Size of the Box
	/// color: Box color
	/// is_box_centered: Use 'position' as the center of the box, not as the bottom corner
	/// duration: Duration of existence in seconds
	void draw_box(Vector3 position, Vector3 size, Color color = Colors::empty_color, bool is_box_centered = false, real_t duration = 0);

	/// Draw box
	/// transform: Transform of the Box
	/// color: Box color
	/// is_box_centered: Use 'transform' as the center of the box, not as the bottom corner
	/// duration: Duration of existence in seconds
	void draw_box_xf(Transform transform, Color color = Colors::empty_color, bool is_box_centered = true, real_t duration = 0);

	/// Draw AABB
	/// aabb: AABB
	/// color: Box color
	/// duration: Duration of existence in seconds
	void draw_aabb(AABB aabb, Color color = Colors::empty_color, real_t duration = 0);

	/// Draw AABB from 'a' to 'b'
	/// a: Firts corner
	/// b: Second corner
	/// color: Box color
	/// duration: Duration of existence in seconds
	void draw_aabb_ab(Vector3 a, Vector3 b, Color color = Colors::empty_color, real_t duration = 0);

#pragma endregion // Boxes
#pragma region Lines

	/// Draw line separated by hit point (billboard square) or not separated if 'is_hit' = 'false'
	/// start: Start point
	/// end: End point
	/// hit: Hit point
	/// is_hit: Is hit
	/// hit_size: Size of the hit point
	/// hit_color: Color of the hit point and line before hit
	/// after_hit_color: Color of line after hit position
	/// duration: Duration of existence in seconds
	void draw_line_hit(Vector3 start, Vector3 end, Vector3 hit, bool is_hit, real_t hit_size = 0.25f, Color hit_color = Colors::empty_color, Color after_hit_color = Colors::empty_color, real_t duration = 0);

	/// Draw line separated by hit point (billboard square) or not separated if 'is_hit' = 'false'
	/// start: Start point
	/// end: End point
	/// is_hit: Is hit
	/// unit_offset_of_hit: Unit offset on the line where the hit occurs
	/// hit_size: Size of the hit point
	/// hit_color: Color of the hit point and line before hit
	/// after_hit_color: Color of line after hit position
	/// duration: Duration of existence in seconds
	void draw_line_hit_offset(Vector3 start, Vector3 end, bool is_hit, real_t unit_offset_of_hit = 0.5f, real_t hit_size = 0.25f, Color hit_color = Colors::empty_color, Color after_hit_color = Colors::empty_color, real_t duration = 0);

#pragma region Normal

	/// Draw line
	/// a: Start point
	/// b: End point
	/// color: Line color
	/// duration: Duration of existence in seconds
	void draw_line(Vector3 a, Vector3 b, Color color = Colors::empty_color, real_t duration = 0);

	/// Draw many lines
	/// lines: Array of line points. 1 line = 2 Vector3. The size of the array must be even.
	/// color: Lines color
	/// duration: Duration of existence in seconds
	void draw_lines(PoolVector3Array lines, Color color = Colors::empty_color, real_t duration = 0);

	/// Draw ray
	/// origin: Origin
	/// direction: Direction
	/// length: Length
	/// color: Ray color
	/// duration: Duration of existence in seconds
	void draw_ray(Vector3 origin, Vector3 direction, real_t length, Color color = Colors::empty_color, real_t duration = 0);

	/// Draw a sequence of points connected by lines
	/// path: Sequence of points
	/// color: Lines Color
	/// duration: Duration of existence in seconds
	void draw_line_path(PoolVector3Array path, Color color = Colors::empty_color, real_t duration = 0);

#pragma endregion // Normal
#pragma region Arrows

	/// Draw arrow
	/// Transform: Transform of the Arrow
	/// color: Arrow color
	/// duration: Duration of existence in seconds
	void draw_arrow(Transform transform, Color color = Colors::empty_color, real_t duration = 0);

	/// Draw line with arrow
	/// a: Start point
	/// b: End point
	/// color: Line color
	/// arrow_size: Size of the arrow
	/// absolute_size: Is the 'arrowSize' absolute or relative to the length of the line?
	/// duration: Duration of existence in seconds
	void draw_arrow_line(Vector3 a, Vector3 b, Color color = Colors::empty_color, real_t arrow_size = 0.5f, bool absolute_size = false, real_t duration = 0);

	/// Draw ray with arrow
	/// origin: Origin
	/// direction: Direction
	/// length: Length
	/// color: Ray color
	/// arrow_size: Size of the arrow
	/// absolute_size: Is the 'arrowSize' absolute or relative to the length of the line?
	/// duration: Duration of existence in seconds
	void draw_arrow_ray(Vector3 origin, Vector3 direction, real_t length, Color color = Colors::empty_color, real_t arrow_size = 0.5f, bool absolute_size = false, real_t duration = 0);

	/// Draw a sequence of points connected by lines with arrows
	/// path: Sequence of points
	/// color: Color
	/// arrow_size: Size of the arrow
	/// absolute_size: Is the 'arrowSize' absolute or relative to the length of the line?
	/// duration: Duration of existence in seconds
	void draw_arrow_path(PoolVector3Array path, Color color = Colors::empty_color, real_t arrow_size = 0.75f, bool absolute_size = true, real_t duration = 0);

#pragma endregion // Arrows
#pragma region Points

	/// Draw a sequence of points connected by lines with billboard squares
	/// path: Sequence of points
	/// points_color: Color of points
	/// lines_color: Color of lines
	/// size: Size of squares
	/// duration: Duration of existence in seconds
	void draw_point_path(PoolVector3Array path, real_t size = 0.25f, Color points_color = Colors::empty_color, Color lines_color = Colors::empty_color, real_t duration = 0);

#pragma endregion // Points
#pragma endregion // Lines
#pragma region Misc

	/// Draw a square that will always be turned towards the camera
	/// position: Center position of square
	/// size: Square size
	/// color: Color
	/// duration: Duration of existence in seconds
	void draw_square(Vector3 position, real_t size = 0.2f, Color color = Colors::empty_color, real_t duration = 0);

	/// Draw a sequence of points using billboard squares
	/// path: Sequence of points
	/// color: Color
	/// size: Size of squares
	/// duration: Duration of existence in seconds
	void draw_points(PoolVector3Array points, real_t size = 0.25f, Color color = Colors::empty_color, real_t duration = 0);

	/// Draw 3 intersecting lines with the given transformations
	/// transform: Transform of lines
	/// color: Color
	/// duration: Duration of existence in seconds
	void draw_position(Transform transform, Color color = Colors::empty_color, real_t duration = 0);

	/// Draw 3 lines with the given transformations and arrows at the ends
	/// transform: Transform of lines
	/// color: Color
	/// is_centered: If 'true', then the lines will intersect in the center of the transform
	/// duration: Duration of existence in seconds
	void draw_gizmo(Transform transform, Color color = Colors::empty_color, bool is_centered = false, real_t duration = 0);

	/// Draw simple grid with given size and subdivision
	/// origin: Grid origin
	/// x_size: Direction and size of the X side. As an axis in the Basis.
	/// y_size: Direction and size of the Y side. As an axis in the Basis.
	/// subdivision: Number of cells for the X and Y axes
	/// color: Lines color
	/// is_centered: Draw lines relative to origin
	/// duration: Duration of existence in seconds
	void draw_grid(Vector3 origin, Vector3 x_size, Vector3 y_size, Vector2 subdivision, Color color = Colors::empty_color, bool is_centered = true, real_t duration = 0);

	/// Draw simple grid with given size and subdivision
	/// transform: Transform of the Grid
	/// subdivision: Number of cells for the X and Y axes
	/// color: Lines color
	/// is_centered: Draw lines relative to origin
	/// duration: Duration of existence in seconds
	void draw_grid_xf(Transform transform, Vector2 subdivision, Color color = Colors::empty_color, bool is_centered = true, real_t duration = 0);

#pragma region Camera Frustum

	/// Draw camera frustum area
	/// camera: Camera node
	/// color: Color
	/// duration: Duration of existence in seconds
	void draw_camera_frustum(class Camera *camera, Color color = Colors::empty_color, real_t duration = 0);

	/// Draw camera frustum area
	/// cameraFrustum: Array of frustum planes
	/// color: Color
	/// duration: Duration of existence in seconds
	void draw_camera_frustum_planes(Array camera_frustum, Color color = Colors::empty_color, real_t duration = 0);

#pragma endregion // Camera Frustum

#pragma endregion // Misc
#pragma endregion // 3D

#pragma region 2D
#pragma region Text

	/// Begin text group
	/// group_title: Group title and ID
	/// group_priority: Group priority
	/// group_color: Group color
	/// show_title: Whether to show the title
	void begin_text_group(String group_title, int group_priority = 0, Color group_color = Colors::empty_color, bool show_title = true);

	/// End text group. Should be called after 'begin_text_group' if you don't need more than one group.
	/// If you need to create 2+ groups just call again 'begin_text_group' and this function in the end.
	void end_text_group();

	/// Add or update text in overlay
	/// key: The name of the field, if there is a 'value', otherwise the whole string will be equal to the 'key'
	/// value: Value of field
	/// priority: Priority of this line. Lower value is higher position.
	/// color_of_value: Value color
	/// duration: Expiration time
	void set_text(String key, Variant value = "", int priority = 0, Color color_of_value = Colors::empty_color, real_t duration = -1);

#pragma endregion // Text
#pragma region Graphs

	/// Create new graph with custom data.
	/// To get more information about the returned class, please see the documentation.
	/// title: Title of the graph
	Ref<GraphParameters> create_graph(String title);

	/// Create new graph with custom data.
	/// To get more information about the returned class, please see the documentation.
	/// title: Title of the graph
	Ref<GraphParameters> create_fps_graph(String title);

	/// Update custom graph data
	/// title: Title of the graph
	/// data: New data
	void graph_update_data(String title, real_t data);

	/// Remove graph
	/// title: Title of the graph
	void remove_graph(String title);

	/// Remove all graphs
	void clear_graphs();

	/// Get config for graph.
	/// To get more information about the returned class, please see the documentation.
	/// title: Title of the graph
	Ref<GraphParameters> get_graph_config(String title);

	/// Get all graph names
	PoolStringArray get_graph_names();

#pragma endregion // Graphs
#pragma endregion // 2D
#pragma endregion // Exposed Draw Functions
};
