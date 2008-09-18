# - Find QRencode
# Find the native QRencode includes and library
#
#  QR_INCLUDE_DIR - where to find qrencode.h, etc.
#  QR_LIBRARIES   - List of libraries when using QRencode.
#  QR_FOUND       - True if QRencode found.


IF (QR_INCLUDE_DIR)
  # Already in cache, be silent
  SET(QR_FIND_QUIETLY TRUE)
ENDIF (QR_INCLUDE_DIR)

FIND_PATH(QR_INCLUDE_DIR qrencode.h)

FIND_LIBRARY(QR_LIBRARY NAMES qrencode )

# handle the QUIETLY and REQUIRED arguments and set QR_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(QR DEFAULT_MSG QR_LIBRARY QR_INCLUDE_DIR)

IF(QR_FOUND)
  SET( QR_LIBRARIES ${QR_LIBRARY} )
ELSE(QR_FOUND)
  SET( QR_LIBRARIES )
ENDIF(QR_FOUND)

MARK_AS_ADVANCED( QR_LIBRARY QR_INCLUDE_DIR )
