// 副作用函数，它是一个无参函数，并带有一个 deps 属性用于存储其所有依赖
type EffectFn = {
  (): void;
  deps: Set<EffectFn>[];
};

// 依赖映射表：Map<属性名, Set<依赖此属性的副作用函数>>
type DepsMap = Map<string | symbol, Set<EffectFn>>;

// 存储副作用函数的桶: WeakMap<目标对象 -> DepsMap>
const bucket = new WeakMap<object, DepsMap>();

// 当前正在执行的副作用函数
let activeEffect: EffectFn | null = null;

/**
 * 副作用函数注册器
 * @param fn 用户定义的副作用函数
 */
function effect(fn: () => void): void {
  const effectFn: EffectFn = () => {
    cleanup(effectFn);
    activeEffect = effectFn;
    fn();
    activeEffect = null;
  };

  effectFn.deps = [];

  effectFn();
}

/**
 * 从所有依赖集合中移除指定的副作用函数
 * @param effectFn 需要被清理的副作用函数
 */
function cleanup(effectFn: EffectFn): void {
  for (let i = 0; i < effectFn.deps.length; i++) {
    const deps = effectFn.deps[i];
    deps!.delete(effectFn);
  }
  effectFn.deps.length = 0;
}

/**
 * 追踪依赖：在 getter 中调用
 * @param target 目标对象
 * @param key 属性名
 */
function track(target: object, key: string | symbol): void {
  if (!activeEffect) return;

  let depsMap = bucket.get(target);
  if (!depsMap) {
    bucket.set(target, (depsMap = new Map()));
  }

  let deps = depsMap.get(key);
  if (!deps) {
    depsMap.set(key, (deps = new Set()));
  }

  deps.add(activeEffect);
  activeEffect.deps.push(deps);
}

/**
 * 触发更新：在 setter 中调用
 * @param target 目标对象
 * @param key 属性名
 */
function trigger(target: object, key: string | symbol): void {
  const depsMap = bucket.get(target);
  if (!depsMap) return;

  const effects = depsMap.get(key);
  if (!effects) return;

  const effectsToRun = new Set(effects);
  effectsToRun.forEach(effectFn => effectFn());
}

/**
 * 创建响应式对象
 * @param obj 普通对象
 * @returns 对象的响应式代理
 */
function reactive<T extends object>(obj: T): T {
  return new Proxy(obj, {
    get(target: T, key: string | symbol, receiver: any): any {
      track(target, key);
      return Reflect.get(target, key, receiver);
    },

    set(target: T, key: string | symbol, newVal: any, receiver: any): boolean {
      const result = Reflect.set(target, key, newVal, receiver);
      trigger(target, key);
      return result;
    },

    has(target: T, key: string | symbol): boolean {
      track(target, key);
      return Reflect.has(target, key);
    },

    ownKeys(target: T): ArrayLike<string | symbol> {
      track(target, Symbol.for('iterate'));
      return Reflect.ownKeys(target);
    },

    deleteProperty(target: T, key: string | symbol): boolean {
      const hadKey = Object.prototype.hasOwnProperty.call(target, key);
      const result = Reflect.deleteProperty(target, key);
      if (result && hadKey) {
        trigger(target, key);
      }
      return result;
    }
  });
}


// --- 使用示例 ---

const data = reactive({
  name: 'John',
  age: 30,
  hobbies: ['reading', 'coding']
});

// 注册副作用函数
effect(() => {
  console.log(`Name: ${data.name}, Age: ${data.age}`);
});

effect(() => {
  console.log(`Hobbies count: ${data.hobbies.length}`);
});

// 修改数据，自动触发副作用函数
console.log('\n--- Modifying data ---');
data.name = 'Jane';  // 输出: Name: Jane, Age: 30
data.age = 25;       // 输出: Name: Jane, Age: 25

// 注意：直接调用 push 会触发两次更新（一次是 'push' 属性的访问，一次是 'length' 属性的修改）
data.hobbies.push('swimming'); // 输出: Hobbies count: 4

// 添加新属性
console.log('\n--- Adding/Deleting properties ---');
(data as any).city = 'New York';

// 删除属性
delete (data as any).age;     // 输出: Name: Jane, Age: undefined