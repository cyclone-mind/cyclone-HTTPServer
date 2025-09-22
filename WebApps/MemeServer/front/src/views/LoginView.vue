<!--
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-22 16:12:13
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-22 16:39:22
 * @FilePath: /cyclone-HTTPServer/WebApps/MemeServer/front/src/views/LoginView.vue
 * @Description:
 * Copyright (c) 2025 by cyclone, All Rights Reserved.
 -->
<template>
  <div class="min-h-screen flex items-center justify-center bg-gray-50 py-12 px-4 sm:px-6 lg:px-8">
    <div class="max-w-md w-full space-y-8">
      <div>
        <div class="mx-auto h-12 w-12 flex items-center justify-center rounded-full bg-purple-100">
          <i data-feather="zap" class="text-purple-600"></i>
        </div>
        <h2 class="mt-6 text-center text-3xl font-extrabold text-gray-900">
          {{ isLogin ? '登录到冲浪' : '注册冲浪账号' }}
        </h2>
        <p class="mt-2 text-center text-sm text-gray-600">
          {{ isLogin ? '还没有账号？' : '已有账号？' }}
          <button @click="toggleMode" class="font-medium text-purple-600 hover:text-purple-500">
            {{ isLogin ? '立即注册' : '立即登录' }}
          </button>
        </p>
      </div>
      <form class="mt-8 space-y-6" @submit.prevent="handleSubmit">
        <div class="rounded-md shadow-sm -space-y-px">
          <div>
            <label for="userId" class="sr-only">用户ID</label>
            <input
              id="userId"
              v-model="form.userId"
              name="userId"
              type="text"
              required
              class="appearance-none rounded-none relative block w-full px-3 py-2 border border-gray-300 placeholder-gray-500 text-gray-900 rounded-t-md focus:outline-none focus:ring-purple-500 focus:border-purple-500 focus:z-10 sm:text-sm"
              placeholder="用户ID"
            />
          </div>
          <div v-if="!isLogin">
            <label for="nickname" class="sr-only">昵称</label>
            <input
              id="nickname"
              v-model="form.nickname"
              name="nickname"
              type="text"
              :required="!isLogin"
              class="appearance-none rounded-none relative block w-full px-3 py-2 border border-gray-300 placeholder-gray-500 text-gray-900 focus:outline-none focus:ring-purple-500 focus:border-purple-500 focus:z-10 sm:text-sm"
              placeholder="昵称"
            />
          </div>
          <div>
            <label for="password" class="sr-only">密码</label>
            <input
              id="password"
              v-model="form.password"
              name="password"
              type="password"
              required
              class="appearance-none rounded-none relative block w-full px-3 py-2 border border-gray-300 placeholder-gray-500 text-gray-900 focus:outline-none focus:ring-purple-500 focus:border-purple-500 focus:z-10 sm:text-sm"
              :class="{ 'rounded-b-md': isLogin }"
              placeholder="密码"
            />
          </div>
          <div v-if="!isLogin">
            <label for="confirmPassword" class="sr-only">确认密码</label>
            <input
              id="confirmPassword"
              v-model="form.confirmPassword"
              name="confirmPassword"
              type="password"
              :required="!isLogin"
              class="appearance-none rounded-none relative block w-full px-3 py-2 border border-gray-300 placeholder-gray-500 text-gray-900 rounded-b-md focus:outline-none focus:ring-purple-500 focus:border-purple-500 focus:z-10 sm:text-sm"
              placeholder="确认密码"
            />
          </div>
        </div>

        <div v-if="isLogin" class="flex items-center justify-between">
          <div class="flex items-center">
            <input
              id="remember-me"
              v-model="form.rememberMe"
              name="remember-me"
              type="checkbox"
              class="h-4 w-4 text-purple-600 focus:ring-purple-500 border-gray-300 rounded"
            />
            <label for="remember-me" class="ml-2 block text-sm text-gray-900">
              记住我
            </label>
          </div>

          <div class="text-sm">
            <a href="#" class="font-medium text-purple-600 hover:text-purple-500">
              忘记密码？
            </a>
          </div>
        </div>

        <div>
          <button
            type="submit"
            :disabled="loading"
            class="group relative w-full flex justify-center py-2 px-4 border border-transparent text-sm font-medium rounded-md text-white bg-purple-600 hover:bg-purple-700 focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-purple-500 disabled:opacity-50"
          >
            <span class="absolute left-0 inset-y-0 flex items-center pl-3">
              <i data-feather="lock" class="h-5 w-5 text-purple-500 group-hover:text-purple-400"></i>
            </span>
            {{ loading ? '处理中...' : (isLogin ? '登录' : '注册') }}
          </button>
        </div>

        <div class="mt-6">
          <div class="relative">
            <div class="absolute inset-0 flex items-center">
              <div class="w-full border-t border-gray-300" />
            </div>
            <div class="relative flex justify-center text-sm">
              <span class="px-2 bg-gray-50 text-gray-500">或者</span>
            </div>
          </div>

          <div class="mt-6">
            <button
              type="button"
              @click="guestLogin"
              class="w-full inline-flex justify-center py-2 px-4 border border-gray-300 rounded-md shadow-sm bg-white text-sm font-medium text-gray-500 hover:bg-gray-50"
            >
              <i data-feather="user" class="h-5 w-5 text-gray-400 mr-2"></i>
              游客模式体验
            </button>
          </div>
        </div>
      </form>
    </div>
  </div>
</template>

<script setup>
import { ref, reactive, onMounted, nextTick } from 'vue'
import { useRouter } from 'vue-router'

const router = useRouter()

// 响应式数据
const isLogin = ref(true)
const loading = ref(false)

const form = reactive({
  userId: '',
  nickname: '',
  password: '',
  confirmPassword: '',
  rememberMe: false
})

// 方法
const toggleMode = () => {
  isLogin.value = !isLogin.value
  // 清空表单
  Object.keys(form).forEach(key => {
    if (typeof form[key] === 'boolean') {
      form[key] = false
    } else {
      form[key] = ''
    }
  })
}

const handleSubmit = async () => {
  if (!isLogin.value) {
    // 注册验证
    if (form.password !== form.confirmPassword) {
      alert('密码不匹配')
      return
    }
    if (form.password.length < 6) {
      alert('密码长度至少6位')
      return
    }
  }

  loading.value = true

  try {
    // 模拟API调用
    await new Promise(resolve => setTimeout(resolve, 1000))
    
    if (isLogin.value) {
      // 登录逻辑
      console.log('登录:', { userId: form.userId, password: form.password })
      alert('登录成功！')
    } else {
      // 注册逻辑
      console.log('注册:', { 
        userId: form.userId, 
        nickname: form.nickname, 
        password: form.password 
      })
      alert('注册成功！请登录')
      isLogin.value = true
      return
    }
    
    // 登录成功后跳转到首页
    router.push('/')
  } catch (error) {
    alert('操作失败，请重试')
    console.error(error)
  } finally {
    loading.value = false
  }
}

const guestLogin = () => {
  // 游客模式直接跳转到首页
  router.push('/')
}

onMounted(() => {
  nextTick(() => {
    if (window.feather) {
      window.feather.replace()
    }
  })
})
</script>