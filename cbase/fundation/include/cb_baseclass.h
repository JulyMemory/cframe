#ifndef CB_BASECLASS_H_
#define CB_BASECLASS_H_

namespace cb {
namespace fundation {
/**
 * @brief ���ɸ�����
 */
class NonCopyable {
 protected:
  NonCopyable(const NonCopyable&) = delete;
  NonCopyable& operator=(const NonCopyable&) = delete;

  NonCopyable() = default;
  ~NonCopyable() = default;
};

}  // namespace fundation
}  // namespace cb

#endif  // CB_BASECLASS_H_
