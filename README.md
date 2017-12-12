# Sofa

A template for a Struct of Array object.

The object also wrapps a dictionary which is used to index the arrays in the object.
The object can be constructed with any number of arrays, but the types must be trivially copyable.
The object will only do one allocationg for all arrays and then divide the allocation between the arrays.
When resizing the arrays only one memcpy is done for each array.

The template is meant to be used in entity component systems.
Each object is a component for an entity, with each array representing for example position or speed.
