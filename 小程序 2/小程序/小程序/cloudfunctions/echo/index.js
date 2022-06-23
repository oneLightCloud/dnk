const cloud = require('wx-server-sdk')

exports.main = async (event, context) => {
  delete event.userInfo
  return event
}
