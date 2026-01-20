"""Generate a clipboard icon for the app."""
from PIL import Image, ImageDraw  # type: ignore  # pylint: disable=import-error

def create_clipboard_icon(size):
    """Create a clipboard icon at the specified size."""
    img = Image.new('RGBA', (size, size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    
    # Scale factor
    s = size / 256
    
    # Colors
    board_color = '#4A90D9'  # Blue clipboard
    board_dark = '#3A7BC8'   # Darker blue for depth
    clip_color = '#FFD93D'   # Gold clip
    clip_dark = '#E6C235'    # Darker gold
    paper_color = '#FFFFFF'  # White paper
    paper_shadow = '#E8E8E8' # Paper shadow
    line_color = '#B0B0B0'   # Text lines
    
    # Clipboard board (rounded rectangle)
    board_left = int(40 * s)
    board_top = int(30 * s)
    board_right = int(216 * s)
    board_bottom = int(240 * s)
    corner_radius = int(20 * s)
    
    # Draw board shadow
    shadow_offset = int(4 * s)
    draw.rounded_rectangle(
        [board_left + shadow_offset, board_top + shadow_offset, 
         board_right + shadow_offset, board_bottom + shadow_offset],
        radius=corner_radius,
        fill='#00000040'
    )
    
    # Draw main board
    draw.rounded_rectangle(
        [board_left, board_top, board_right, board_bottom],
        radius=corner_radius,
        fill=board_color
    )
    
    # Draw board edge (3D effect)
    draw.rounded_rectangle(
        [board_left, board_top, board_right, board_bottom],
        radius=corner_radius,
        outline=board_dark,
        width=int(3 * s)
    )
    
    # Paper on clipboard
    paper_margin = int(20 * s)
    paper_left = board_left + paper_margin
    paper_top = board_top + int(40 * s)
    paper_right = board_right - paper_margin
    paper_bottom = board_bottom - paper_margin
    
    # Paper shadow
    draw.rectangle(
        [paper_left + int(2*s), paper_top + int(2*s), 
         paper_right + int(2*s), paper_bottom + int(2*s)],
        fill=paper_shadow
    )
    
    # Paper
    draw.rectangle(
        [paper_left, paper_top, paper_right, paper_bottom],
        fill=paper_color
    )
    
    # Text lines on paper
    line_start = paper_left + int(15 * s)
    line_end = paper_right - int(15 * s)
    line_y_start = paper_top + int(25 * s)
    line_spacing = int(22 * s)
    line_height = int(4 * s)
    
    for i in range(6):
        y = line_y_start + i * line_spacing
        # Vary line lengths for realism
        end_offset = int(20 * s) if i % 2 == 0 else int(40 * s) if i == 3 else 0
        if y + line_height < paper_bottom - int(15 * s):
            draw.rectangle(
                [line_start, y, line_end - end_offset, y + line_height],
                fill=line_color
            )
    
    # Clip at top
    clip_width = int(80 * s)
    clip_height = int(50 * s)
    clip_left = (board_left + board_right - clip_width) // 2
    clip_top = int(10 * s)
    clip_right = clip_left + clip_width
    clip_bottom = clip_top + clip_height
    clip_radius = int(8 * s)
    
    # Clip shadow
    draw.rounded_rectangle(
        [clip_left + int(2*s), clip_top + int(2*s), 
         clip_right + int(2*s), clip_bottom + int(2*s)],
        radius=clip_radius,
        fill='#00000030'
    )
    
    # Main clip
    draw.rounded_rectangle(
        [clip_left, clip_top, clip_right, clip_bottom],
        radius=clip_radius,
        fill=clip_color
    )
    
    # Clip highlight
    draw.rounded_rectangle(
        [clip_left, clip_top, clip_right, clip_bottom],
        radius=clip_radius,
        outline=clip_dark,
        width=int(2 * s)
    )
    
    # Inner clip hole
    hole_width = int(40 * s)
    hole_height = int(20 * s)
    hole_left = (clip_left + clip_right - hole_width) // 2
    hole_top = clip_top + int(20 * s)
    hole_radius = int(5 * s)
    
    draw.rounded_rectangle(
        [hole_left, hole_top, hole_left + hole_width, hole_top + hole_height],
        radius=hole_radius,
        fill=board_color
    )
    
    return img

def main():
    # Create icons at multiple sizes for ICO file
    sizes = [256, 128, 64, 48, 32, 16]
    icons = [create_clipboard_icon(size) for size in sizes]
    
    # Save as ICO
    icons[0].save(
        'resources/app_icon.ico',
        format='ICO',
        sizes=[(s, s) for s in sizes],
        append_images=icons[1:]
    )
    
    # Also save a PNG for other uses
    icons[0].save('resources/app_icon.png', format='PNG')
    
    print("Created resources/app_icon.ico and resources/app_icon.png")

if __name__ == '__main__':
    import os
    os.makedirs('resources', exist_ok=True)
    main()
