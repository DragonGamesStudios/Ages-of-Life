Notes for future me, who won't remember what is what:
--
### 1. Game data structure: POR

POR is Prototype - Object - Reference structure. First, the prototypes are built. They are simple structs, that hold basic data about an object. Basing on them the objects
are initialized. Later, their dependencies are filled (such as next age pointer etc.). The civilizations don't store objects, but references - structs containing pointer to object
and parameters, that are dependent on civilization (technology.research etc.). This structure provides flexibility and efficiency (I suppose).

### 2. Prototypes
a) Age prototype:
name - object name
neighbor_age - one of the neighboring ages - you can either provide previous or the next one
icon_path - path to age icon
master_image_path - path to technology label image
font_color - technology text color
master_color - I'm not sure
neighbor_specifier - specifies if the given neighbor age is after the selected one or before it

### 3. Important structures
a) TechnologyPath - vector of Technology* used to define which technologies should be researched in order to research certain technology. It's sorted using following rules:
-> technology of lower level is earlier
-> if two technologies are the same level they are sorted by name

## Proto

### 1. DOM GUI

Pretty simmilar to DOM design. Division into elements and
controlling them with rulesets - for documentation, just look
at css docs.

Notes:
- Border corners are not displayed properly if different colors/styles
- If border is set to none, but still has width, it is "displayed"
- `background-size` property needs two values for numeric and one for keyword (background-size: 75%; is not correct)
- `background-position` is just a shorthand for `background-position-x` and `y`
- `background-repear` is just a shorthand for `background-repeat-x` and `y`