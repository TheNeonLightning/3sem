Stack with protection:

• Used different techniques to prevent stack data from injections.
  1) Checking hash value which is calculated on the buffer and on the stack parameters simultaneously.
  2) Filling the unused part of the buffer with specially generated poison value. Checking that the unused part of the buffer is not being changed.
  3) Maintaining sentinels on the edges of data contained in buffer.

• Implemented the dumper for the stack, containing all necessary information.

• Developed an optimal stack representation method.

• Added documentation for the project.

(Additionally) Cross check carried out with the protected stack of other student: https://github.com/Shushpancheak/shush-stack
