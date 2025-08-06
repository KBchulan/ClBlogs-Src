interface User {
  name: string
  age: number
  email: string
}

// Partial, 成员都变为可选
type PartialUser = Partial<User>

// Required, 成员都变为必选
  type RequiredUser = Required<PartialUser>

// Pick, 选择部分成员
type PickUser = Pick<User, 'name' | 'email'>

// Omit, 排除部分成员
type OmitUser = Omit<User, 'age' | 'email'>

// Exclude, 排除部分成员，返回值为一个联合类型
type ExcludeUser = Exclude<keyof User, 'age'>

// Record，约束键和值的类型
type UserRecord = Record<'name' | 'email', string>

// ReturnType，获取函数返回值的类型
const getArr = () => [1, 2, 3]
type ArrType = ReturnType<typeof getArr>