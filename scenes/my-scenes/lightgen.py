from xml.etree import ElementTree as ET

# Define grid dimensions and spacing
grid_size = 7
spacing = 25

# Create the root element with indentation
root = ET.Element("Scene", attrib={"groundPlane": "true"})
indent = "  "  # Indentation string

# Loop through the grid and create lights
for i in range(grid_size):
    for j in range(grid_size):
        # Calculate light position based on grid coordinates (shifted for better distribution)
        x = (i / (grid_size - 1.0)) * spacing * 2.0 - spacing  # Adjusted calculation using spacing
        y = 1.0
        z = (j / (grid_size - 1.0)) * spacing * 2.0 - spacing  # Adjusted calculation using spacing

        # Define light color based on position
        color_index = (i + j) % 4
        if color_index == 0:
            diffuse_color = "1 0 0 1"
        elif color_index == 1:
            diffuse_color = "0 0 1 1"
        elif color_index == 2:
            diffuse_color = "1 0 1 1"
        else:
            diffuse_color = "0 1 0 1"

        # Create the transform element with indentation
        transform = ET.SubElement(root, "Transform", attrib={"name": "Transform1", 
                                                                "scale": "1 1 1", 
                                                                "translate": f"{x} {y} {z}", 
                                                                "rotate": "0 0 0"})
        transform.tail = "\n" + indent  # Add newline and indent for next element

        # Create the light element with indentation
        light = ET.SubElement(transform, "Light", attrib={"position": "0 0 0 1", "diffuse": diffuse_color,
                                                            "specular": "1 1 1 1"})
        light.tail = "\n"  # Add newline after light element

# Write the XML content to a file with formatting
with open("lights.xml", "wb") as f:
    f.write(ET.tostring(root, encoding="utf-8", method="xml"))

print("XML file generated successfully with formatting!")

